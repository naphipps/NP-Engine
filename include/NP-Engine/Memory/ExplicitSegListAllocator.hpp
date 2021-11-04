//
//  ExplicitSegListAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 3/1/21.
//

#ifndef NP_ENGINE_EXPLICIT_SEG_LIST_ALLOCATOR_HPP
#define NP_ENGINE_EXPLICIT_SEG_LIST_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "ExplicitListAllocator.hpp"
#include "SizedAllocator.hpp"
#include "Margin.hpp"

namespace np
{
    namespace memory
    {
        class ExplicitSegListAllocator : public SizedAllocator
        {
        private:
            using Node = hidden::ExplicitListAllocatorNode;
            using NodePtr = Node*;
            using Margin = hidden::Margin;
            using MarginPtr = hidden::MarginPtr;
            
            constexpr static siz NODE_ALIGNED_SIZE = CalcAlignedSize(sizeof(Node));
            constexpr static siz OVERHEAD_ALIGNED_SIZE = (hidden::MARGIN_ALIGNED_SIZE << 1) + NODE_ALIGNED_SIZE;
            
            Mutex _mutex;
            
            //free ptr are from aligned sizes A to B
            NodePtr _root_1_16;
            NodePtr _root_17_32;
            NodePtr _root_33_64;
            NodePtr _root_65_;
            
            /**
             gets the root pointer for the given require aligned size
             */
            NodePtr* GetRoot(siz required_aligned_size)
            {
                NodePtr* root = nullptr;
                
                if (_root_1_16 != nullptr && required_aligned_size <= 16)
                {
                    root = &_root_1_16;
                }
                else if (_root_17_32 != nullptr && required_aligned_size <= 32)
                {
                    root = &_root_17_32;
                }
                else if (_root_33_64 != nullptr && required_aligned_size <= 64)
                {
                    root = &_root_33_64;
                }
                else
                {
                    root = &_root_65_;
                }
                
                return root;
            }
            
            /**
             gets the address of the root pointer that matches the given root value
             this is only useful in our stowing method
             */
            NodePtr* GetRootMatching(NodePtr ptr)
            {
                NodePtr* root = nullptr;
                
                if (_root_1_16 == ptr)
                {
                    root = &_root_1_16;
                }
                else if (_root_17_32 == ptr)
                {
                    root = &_root_17_32;
                }
                else if (_root_33_64 == ptr)
                {
                    root = &_root_33_64;
                }
                else if (_root_65_ == ptr)
                {
                    root = &_root_65_;
                }
                
                return root;
            }
            
            /**
             detaches our node from the double linked list
             */
            void DetachNode(NodePtr node, NodePtr* root)
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
                    *root = node->Next;
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
             we also consider if the block is attached to a list or not
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
                    for (NodePtr it = *GetRoot(block.size);
                         Contains(it); it = it->Next)
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
                        *GetRoot(block.size) = node;
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
                    for (NodePtr it = *GetRoot(required_aligned_size);
                         _block.Contains(it); it = it->Next)
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
                    for (NodePtr it = *GetRoot(required_aligned_size);
                         _block.Contains(it); it = it->Next)
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
                    
                    DetachNode(node, GetRoot(required_alloc_size));
                    
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
                _root_1_16 = nullptr;
                _root_17_32 = nullptr;
                _root_33_64 = nullptr;
                _root_65_ = nullptr;
                
                if (InitFreeBlock(_block))
                {
                    *GetRoot(_block.size) = (NodePtr)((ui8*)_block.Begin() + hidden::MARGIN_ALIGNED_SIZE);
                }
            }
            
        public:
            
            /**
             constructor
             */
            ExplicitSegListAllocator(Block block):
            SizedAllocator(block),
            _root_1_16(nullptr),
            _root_17_32(nullptr),
            _root_33_64(nullptr),
            _root_65_(nullptr)
            {
                Init();
            }
            
            /**
             constructor
             */
            ExplicitSegListAllocator(siz size):
            SizedAllocator(size),
            _root_1_16(nullptr),
            _root_17_32(nullptr),
            _root_33_64(nullptr),
            _root_65_(nullptr)
            {
                Init();
            }
            
            /**
             deconstructor
             */
            ~ExplicitSegListAllocator() = default;
            
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
                                
                                NodePtr claim_node = (NodePtr)((ui8*)claim_header + hidden::MARGIN_ALIGNED_SIZE);
                                DetachNode(claim_node, GetRoot(claim_header->GetSize()));
                                
                                claim_header->SetSize(claim_header->GetSize() + header->GetSize());
                                MarginPtr claim_footer = (MarginPtr)((ui8*)claim_header + claim_header->GetSize() - hidden::MARGIN_ALIGNED_SIZE);
                                claim_footer->Value = claim_header->Value;
                                header = claim_header;
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
                                
                                NodePtr claim_node = (NodePtr)((ui8*)claim_header + hidden::MARGIN_ALIGNED_SIZE);
                                DetachNode(claim_node, GetRoot(claim_header->GetSize()));
                                
                                header->SetSize(header->GetSize() + claim_header->GetSize());
                                MarginPtr claim_footer = (MarginPtr)((ui8*)header + header->GetSize() - hidden::MARGIN_ALIGNED_SIZE);
                                claim_footer->Value = header->Value;
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

#endif /* NP_ENGINE_EXPLICIT_SEG_LIST_ALLOCATOR_HPP */
