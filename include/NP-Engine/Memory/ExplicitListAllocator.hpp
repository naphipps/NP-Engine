//
//  ExplicitListAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/25/21.
//

#ifndef NP_ENGINE_EXPLICIT_LIST_ALLOCATOR_HPP
#define NP_ENGINE_EXPLICIT_LIST_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "SizedAllocator.hpp"
#include "Margin.hpp"

namespace np
{
    namespace memory
    {
        namespace hidden
        {
            class ExplicitListAllocatorNode
            {
            private:
                using Node = ExplicitListAllocatorNode;
                using NodePtr = Node*;
                
            public:
                
                NodePtr Next = nullptr;
                NodePtr Prev = nullptr;
            };
        }
        
        class ExplicitListAllocator : public SizedAllocator
        {
        private:
            using Node = hidden::ExplicitListAllocatorNode;
            using NodePtr = Node*;
            using Margin = hidden::Margin;
            using MarginPtr = hidden::MarginPtr;
            
            constexpr static siz NODE_ALIGNED_SIZE = CalcAlignedSize(sizeof(Node));
            constexpr static siz OVERHEAD_ALIGNED_SIZE = (hidden::MARGIN_ALIGNED_SIZE << 1) + NODE_ALIGNED_SIZE;
            
            Mutex _mutex;
            NodePtr _root;
            
            /**
             detaches our node from the double linked list
             */
            void DetachNode(NodePtr node)
            {
                if (node->Next != nullptr)
                {
                    node->Next->Prev = node->Prev;
                }
                
                if (node->Prev != nullptr)
                {
                    node->Prev->Next = node->Next;
                }
                else
                {
                    _root = node->Next;
                }
            }
            
            /**
             inits the given block with our free block overhead
             */
            bl InitFreeBlock(Block& block)
            {
                bl initialized = false;
                
                if (block.size >= OVERHEAD_ALIGNED_SIZE)
                {
                    Block header_block{block.Begin(), hidden::MARGIN_ALIGNED_SIZE};
                    Block footer_block{(ui8*)block.End() - hidden::MARGIN_ALIGNED_SIZE, hidden::MARGIN_ALIGNED_SIZE};
                    Construct<Margin>(header_block);
                    Construct<Margin>(footer_block);
                    MarginPtr header = (MarginPtr)header_block.Begin();
                    MarginPtr footer = (MarginPtr)footer_block.Begin();
                    header->SetSize(block.size);
                    header->SetDeallocated();
                    footer->Value = header->Value;
                    
                    Block node_block{header_block.End(), NODE_ALIGNED_SIZE};
                    Construct<Node>(node_block);
                    
                    initialized = true;
                }
                else
                {
                    block.Zeroize();
                }
                
                return initialized;
            }
            
            /**
             stows the given free block in the correct place
             */
            void StowFreeBlock(Block& block)
            {
                //init our block as free
                bl init_success = InitFreeBlock(block);
                NP_ASSERT(init_success, "our init here should always succeed");
                NodePtr node = (NodePtr)((ui8*)block.Begin() + hidden::MARGIN_ALIGNED_SIZE);
                
                //store block in next_free's list
                {
                    NodePtr insert = nullptr;
                    for (NodePtr it = _root; Contains(it); it = it->Next)
                    {
                        insert = it;
                        
                        if (it->Next > node->Next)
                        {
                            break;
                        }
                    }
                    
                    node->Next = insert != nullptr ? insert->Next : nullptr;
                    node->Prev = insert;
                    
                    if (node->Next != nullptr)
                    {
                        node->Next->Prev = node;
                    }
                    
                    if (node->Prev != nullptr)
                    {
                        node->Prev->Next = node;
                    }
                    else
                    {
                        _root = node;
                    }
                }
            }
            
            /**
             find the allocation that fits the given required size using the specified best or first search
             */
            ui8* FindAllocation(siz required_aligned_size, bl true_best_false_first)
            {
                ui8* allocation = nullptr;
                
                if (true_best_false_first)
                {
                    siz min_diff = _block.size + 1;
                    for (NodePtr it = _root; _block.Contains(it); it = it->Next)
                    {
                        MarginPtr header = (MarginPtr)((ui8*)it - hidden::MARGIN_ALIGNED_SIZE);
                        if (!header->IsAllocated())
                        {
                            siz diff = header->GetSize() - required_aligned_size;
                            if (diff == 0)
                            {
                                allocation = (ui8*)header;
                                break;
                            }
                            else if (diff < min_diff)
                            {
                                allocation = (ui8*)header;
                                min_diff = diff;
                            }
                        }
                    }
                }
                else
                {
                    for (NodePtr it = _root; _block.Contains(it); it = it->Next)
                    {
                        MarginPtr header = (MarginPtr)((ui8*)it - hidden::MARGIN_ALIGNED_SIZE);
                        if (!header->IsAllocated() && header->GetSize() >= required_aligned_size)
                        {
                            allocation = (ui8*)header;
                            break;
                        }
                    }
                }
                
                return allocation;
            }
            
            /**
             allocates a block the given size will fit inside
             we search for a block using the specified best or first search
             */
            Block Allocate(siz size, bl true_best_false_first)
            {
                Lock lock(_mutex);
                Block block;
                siz required_alloc_size = CalcAlignedSize(size) + (hidden::MARGIN_ALIGNED_SIZE << 1);
                if (required_alloc_size < OVERHEAD_ALIGNED_SIZE)
                {
                    required_alloc_size = OVERHEAD_ALIGNED_SIZE;
                }
                ui8* alloc = FindAllocation(required_alloc_size, true_best_false_first);
                
                if (alloc != nullptr)
                {
                    MarginPtr header = (MarginPtr)alloc;
                    NodePtr node = (NodePtr)(alloc + hidden::MARGIN_ALIGNED_SIZE);
                    Block alloc_block{header, header->GetSize()};
                    
                    DetachNode(node);
                    
                    //can we split?
                    if (header->GetSize() - required_alloc_size >= OVERHEAD_ALIGNED_SIZE)
                    {
                        Block split_block
                        {
                            alloc + required_alloc_size,
                            header->GetSize() - required_alloc_size
                        };
                        alloc_block.size -= split_block.size;
                        StowFreeBlock(split_block);
                        
                        bl alloc_success = InitFreeBlock(alloc_block);
                        NP_ASSERT(alloc_success, "alloc_success must happen here");
                    }
                    
                    header->SetAllocated();
                    MarginPtr footer = (MarginPtr)((ui8*)alloc_block.End() - hidden::MARGIN_ALIGNED_SIZE);
                    footer->Value = header->Value;
                    
                    block.ptr = (ui8*)alloc_block.Begin() + hidden::MARGIN_ALIGNED_SIZE;
                    block.size = alloc_block.size - (hidden::MARGIN_ALIGNED_SIZE << 1);
                }
                
                return block;
            }
            
            /**
             init
             */
            void Init()
            {
                _root = InitFreeBlock(_block) ? (NodePtr)((ui8*)_block.Begin() + hidden::MARGIN_ALIGNED_SIZE) : nullptr;
            }
            
        public:
            
            /**
             constructor
             */
            ExplicitListAllocator(Block block):
            SizedAllocator(block),
            _root(nullptr)
            {
                Init();
            }
            
            /**
             constructor
             */
            ExplicitListAllocator(siz size):
            SizedAllocator(size),
            _root(nullptr)
            {
                Init();
            }
            
            /**
             deconstructor
             */
            ~ExplicitListAllocator() = default;
            
            /**
             allocates a block of memory the given size will fit inside
             */
            Block Allocate(siz size) override
            {
                return AllocateBest(size);
            }
            
            /**
             allocates the best fitting block of memory for the given size
             */
            Block AllocateBest(siz size)
            {
                return Allocate(size, true);
            }
            
            /**
             allocates the first block of memory that can fit the given size
             */
            Block AllocateFirst(siz size)
            {
                return Allocate(size, false);
            }
            
            /**
             deallocates the given block
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
                    MarginPtr header = (MarginPtr)((ui8*)ptr - hidden::MARGIN_ALIGNED_SIZE);
                    header->SetDeallocated();
                    
                    //coalesce
                    {
                        //claim previous blocks
                        for (void* prev_footer = (ui8*)header - hidden::MARGIN_ALIGNED_SIZE;
                             Contains(prev_footer);
                             prev_footer = (ui8*)header - hidden::MARGIN_ALIGNED_SIZE)
                        {
                            if (((MarginPtr)prev_footer)->IsAllocated())
                            {
                                break;
                            }
                            else
                            {
                                //claim header
                                MarginPtr claim_header = (MarginPtr)((ui8*)prev_footer + hidden::MARGIN_ALIGNED_SIZE - ((MarginPtr)prev_footer)->GetSize());
                                claim_header->SetSize(claim_header->GetSize() + header->GetSize());
                                MarginPtr claim_footer = (MarginPtr)((ui8*)claim_header + claim_header->GetSize() - hidden::MARGIN_ALIGNED_SIZE);
                                claim_footer->Value = claim_header->Value;
                                header = claim_header;

                                NodePtr claim_node = (NodePtr)((ui8*)claim_header + hidden::MARGIN_ALIGNED_SIZE);
                                DetachNode(claim_node);
                            }
                        }
                                                
                        //claim next blocks
                        for (void* next_header = (ui8*)header + header->GetSize();
                             Contains(next_header);
                             next_header = (ui8*)header + header->GetSize())
                        {
                            if (((MarginPtr)next_header)->IsAllocated())
                            {
                                break;
                            }
                            else
                            {
                                //claim header
                                MarginPtr claim_header = (MarginPtr)next_header;
                                header->SetSize(header->GetSize() + claim_header->GetSize());
                                MarginPtr claim_footer = (MarginPtr)((ui8*)header + header->GetSize() - hidden::MARGIN_ALIGNED_SIZE);
                                claim_footer->Value = header->Value;

                                NodePtr claim_node = (NodePtr)((ui8*)claim_header + hidden::MARGIN_ALIGNED_SIZE);
                                DetachNode(claim_node);
                            }
                        }
                    }
                    
                    Block dealloc_block{header, header->GetSize()};
                    StowFreeBlock(dealloc_block);
                    deallocated = true;
                }
                
                return deallocated;
            }
            
            /**
             deallocates all of our block of memory
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

#endif /* NP_ENGINE_EXPLICIT_LIST_ALLOCATOR_HPP */
