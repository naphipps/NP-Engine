//
//  ImplicitListAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/19/21.
//

#ifndef NP_ENGINE_IMPLICIT_ALLOCATOR_HPP
#define NP_ENGINE_IMPLICIT_ALLOCATOR_HPP

#include "NP-Engine/Insight/Insight.hpp"

#include "SizedAllocator.hpp"
#include "Margin.hpp"

namespace np
{
    namespace memory
    {
        class ImplicitListAllocator : public SizedAllocator
        {
        private:
            using Margin = __detail::Margin;
            using MarginPtr = __detail::MarginPtr;
            
            constexpr static siz OVERHEAD_ALIGNED_SIZE = (__detail::MARGIN_ALIGNED_SIZE << 1);
            
            Mutex _mutex;
            
            /**
             inits the given block with our free block overhead
             */
            bl InitFreeBlock(Block& block)
            {
                bl initialized = false;
                
                if (block.size >= OVERHEAD_ALIGNED_SIZE)
                {
                    Block header_block{block.Begin(), __detail::MARGIN_ALIGNED_SIZE};
                    Block footer_block{(ui8*)block.End() - __detail::MARGIN_ALIGNED_SIZE, __detail::MARGIN_ALIGNED_SIZE};
                    Construct<Margin>(header_block);
                    Construct<Margin>(footer_block);
                    MarginPtr header = (MarginPtr)header_block.ptr;
                    MarginPtr footer = (MarginPtr)footer_block.ptr;
                    header->SetSize(block.size);
                    header->SetDeallocated();
                    footer->Value = header->Value;
                    initialized = true;
                }
                else
                {
                    block.Zeroize();
                }
                
                return initialized;
            }
            
            /**
             find the allocation that fits the given required size using the specified best or first search
             */
            ui8* FindAllocation(siz required_ALIGNED_SIZE, bl true_best_false_first)
            {
                ui8* allocation = nullptr;
                                
                if (true_best_false_first)
                {
                    siz min_diff = _block.size + 1;
                    for (ui8* it = static_cast<ui8*>(_block.Begin());
                         _block.Contains(it);
                         it += ((MarginPtr)it)->GetSize())
                    {
                        if (!((MarginPtr)it)->IsAllocated())
                        {
                            siz diff = ((MarginPtr)it)->GetSize() - required_ALIGNED_SIZE;
                            if (diff == 0)
                            {
                                allocation = it;
                                break;
                            }
                            else if (diff < min_diff)
                            {
                                allocation = it;
                                min_diff = diff;
                            }
                        }
                    }
                }
                else
                {
                    for (ui8* it = static_cast<ui8*>(_block.Begin());
                         _block.Contains(it);
                         it += ((MarginPtr)it)->GetSize())
                    {
                        if (!((MarginPtr)it)->IsAllocated() &&
                            ((MarginPtr)it)->GetSize() >= required_ALIGNED_SIZE)
                        {
                            allocation = it;
                            break;
                        }
                    }
                }
                
                return allocation;
            }
            
            /**
             allocates a block the given size can fit inside and given the search method
             use true to use best searching and use false to use first searching
             */
            Block Allocate(siz size, bl true_best_false_first)
            {
                Lock lock(_mutex);
                Block block;
                siz required_alloc_size = CalcAlignedSize(size) + (__detail::MARGIN_ALIGNED_SIZE << 1);
                ui8* alloc = FindAllocation(required_alloc_size, true_best_false_first);
                
                if (alloc != nullptr)
                {
                    MarginPtr header = (MarginPtr)alloc;
                    Block alloc_block{header, header->GetSize()};
                    
                    if (header->GetSize() - required_alloc_size >= OVERHEAD_ALIGNED_SIZE)
                    {
                        Block split_block
                        {
                            alloc + required_alloc_size,
                            header->GetSize() - required_alloc_size
                        };
                        bl split_success = InitFreeBlock(split_block);
                        NP_ASSERT(split_success, "split must succeed here");
                        alloc_block.size -= split_block.size;
                    }
                    
                    if (header->GetSize() != alloc_block.size)
                    {
                        bl alloc_success = InitFreeBlock(alloc_block);
                        NP_ASSERT(alloc_success, "alloc_success must happen here");
                    }
                    
                    header->SetAllocated();
                    MarginPtr footer = (MarginPtr)((ui8*)alloc_block.End() - __detail::MARGIN_ALIGNED_SIZE);
                    footer->Value = header->Value;
                    
                    block.ptr = (ui8*)alloc_block.Begin() + __detail::MARGIN_ALIGNED_SIZE;
                    block.size = alloc_block.size - (__detail::MARGIN_ALIGNED_SIZE << 1);
                }
                
                return block;
            }
            
            /**
             initializes our initial margins
             */
            void Init()
            {
                InitFreeBlock(_block);
            }
            
        public:
            
            /**
             constructor
             */
            ImplicitListAllocator(Block block):
            SizedAllocator(block)
            {
                Init();
            }
            
            /**
             constructor
             */
            ImplicitListAllocator(siz size):
            SizedAllocator(size)
            {
                Init();
            }
            
            /**
             deconstructor
             */
            ~ImplicitListAllocator() = default;
            
            /**
             allocates a block of memory the given size can fit inside
             */
            Block Allocate(siz size) override
            {
                return AllocateBest(size);
            }
                 
            /**
             allocates the first block of memory the given size can fit inside
             */
            Block AllocateFirst(siz size)
            {
                return Allocate(size, false);
            }
            
            /**
             allocates the best block of memory the given size can fit inside
             */
            Block AllocateBest(siz size)
            {
                return Allocate(size, true);
            }
            
            /**
             deallocates the given block if it belongs to us
             */
            bl Deallocate(Block& block) override
            {
                bl deallocated = Deallocate(block.ptr);
                if (deallocated)
                {
                    block.Invalidate();
                }
                return deallocated;
            }
            
            /**
             deallocates a block given the ptr
             */
            bl Deallocate(void* ptr) override
            {
                Lock lock(_mutex);
                bl deallocated = false;
                
                if (Contains(ptr))
                {
                    MarginPtr header = (MarginPtr)((ui8*)ptr - __detail::MARGIN_ALIGNED_SIZE);
                    MarginPtr footer = (MarginPtr)((ui8*)header + header->GetSize() - __detail::MARGIN_ALIGNED_SIZE);
                    
                    //claim previous blocks
                    for (ui8* prev_footer = (ui8*)header - __detail::MARGIN_ALIGNED_SIZE;
                         Contains(prev_footer);
                         prev_footer = (ui8*)header - __detail::MARGIN_ALIGNED_SIZE)
                    {
                        if (((MarginPtr)prev_footer)->IsAllocated())
                        {
                            break;
                        }
                        else
                        {
                            header = (MarginPtr)(prev_footer + __detail::MARGIN_ALIGNED_SIZE - ((MarginPtr)prev_footer)->GetSize());
                        }
                    }

                    //claim next blocks
                    for (ui8* next_header = (ui8*)footer + __detail::MARGIN_ALIGNED_SIZE;
                         Contains(next_header);
                         next_header = (ui8*)footer + __detail::MARGIN_ALIGNED_SIZE)
                    {
                        if (((MarginPtr)next_header)->IsAllocated())
                        {
                            break;
                        }
                        else
                        {
                            footer = (MarginPtr)(next_header - __detail::MARGIN_ALIGNED_SIZE + ((MarginPtr)next_header)->GetSize());
                        }
                    }
                    
                    Block dealloc_block{header, (ui8*)footer - (ui8*)header + __detail::MARGIN_ALIGNED_SIZE};
                    bl dealloc_success = InitFreeBlock(dealloc_block);
                    NP_ASSERT(dealloc_success, "we should have a successful deallocation here");
                    deallocated = true;
                }
                
                return deallocated;
            }
            
            /**
             deallocates all memory
             */
            bl DeallocateAll() override
            {
                Lock lock(_mutex);
                Init();
                return true;
            }
        };
    }
}

#endif /* NP_ENGINE_IMPLICIT_ALLOCATOR_HPP */
