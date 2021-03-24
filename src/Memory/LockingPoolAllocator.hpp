//
//  LockingPoolAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 3/6/21.
//

#ifndef NP_ENGINE_LOCKING_POOL_ALLOCATOR_HPP
#define NP_ENGINE_LOCKING_POOL_ALLOCATOR_HPP

#include "Primitive/Primitive.hpp"

#include "SizedAllocator.hpp"

namespace np
{
    namespace memory
    {
        template <typename T>
        class LockingPoolAllocator : public SizedAllocator
        {
        public:
            constexpr static siz CHUNK_ALIGNED_SIZE = CalcAlignedSize(sizeof(T));
            
        private:
            Mutex _mutex;
            void* _alloc_iterator;
            bl _deallocation_true_sort_false_constant;
            
            /**
             evaluates if the given ptr points to the beginning of a chunk or not
             */
            bl IsChunkPtr(void* ptr) const
            {
                return Contains(ptr) && ((ui8*)ptr - (ui8*)_block.Begin()) % CHUNK_ALIGNED_SIZE == 0;
            }
            
            /**
             initializes this pool
             */
            void Init()
            {
                Block block{.size = CHUNK_ALIGNED_SIZE};
                
                for (ui32 i=0; i<ChunkCount()-1; i++)
                {
                    block.ptr = &static_cast<ui8*>(_block.ptr)[i * CHUNK_ALIGNED_SIZE];
                    Construct<void*>(block, &static_cast<ui8*>(_block.ptr)[(i + 1) * CHUNK_ALIGNED_SIZE]);
                }
                
                block.ptr = &static_cast<ui8*>(_block.ptr)[(ChunkCount()-1) * CHUNK_ALIGNED_SIZE];
                Construct<void*>(block, nullptr);
                
                _alloc_iterator = _block.ptr;
            }
            
        public:
            
            /**
             constructor
             */
            LockingPoolAllocator(Block block):
            SizedAllocator(block),
            _deallocation_true_sort_false_constant(false)
            {
                Init();
            }
            
            /**
             constructor
             */
            LockingPoolAllocator(siz size):
            SizedAllocator(size),
            _deallocation_true_sort_false_constant(false)
            {
                Init();
            }
            
            /**
             deconstructor
             */
            ~LockingPoolAllocator() = default;
            
            /**
             gets the count of chunk in this pool
             */
            siz ChunkCount() const
            {
                return _block.size / CHUNK_ALIGNED_SIZE;
            }

            /**
             gets the chunk size
             */
            siz ChunkSize() const
            {
                return CHUNK_ALIGNED_SIZE;
            }
            
            /**
             sets the default deallocation reclaim behavior
             true means we will use sort insertion by default
             false means we will use constant behavior by default
             */
            bl GetDefaultDeallocationReclaimBehavior() const
            {
                return _deallocation_true_sort_false_constant;
            }
            
            /**
             sets the default deallocation reclaim behavior
             true means we will use sort insertion by default
             false means we will use constant behavior by default
             the defalt param value of false is our default behavior
             */
            void SetDefaultDeallocationReclaimBehavior(bl true_sort_false_constant = false)
            {
                _deallocation_true_sort_false_constant = true_sort_false_constant;
            }
            
            /**
             zeroizes our block and initializes the chunks
             */
            void Zeroize() override
            {
                Lock lock(_mutex);
                SizedAllocator::Zeroize();
                Init();
            }
            
            /**
             allocates a chunk sized block that the given size can fit inside, or returns an invalid block
             */
            Block Allocate(siz size) override
            {
                Lock lock(_mutex);
                Block block;
                
                if (size <= CHUNK_ALIGNED_SIZE && _alloc_iterator != nullptr)
                {
                    block.ptr = _alloc_iterator;
                    block.size = CHUNK_ALIGNED_SIZE;
                    _alloc_iterator = *(void**)(&static_cast<ui8*>(_alloc_iterator)[0]);
                }
                
                return block;
            }
            
            /**
             deallocates the given block if we contain it and we insert the new free block back using the specified option
             if true we use a sorted insertion based off the block's address to store the new free block
             if false we simply place the new free block as the next block to be allocated - this is constant
             */
            bl Deallocate(Block& block, bl true_sort_false_constant)
            {
                Lock lock(_mutex);
                bl deallocated = false;
                
                if (IsChunkPtr(block.ptr) && block.size == CHUNK_ALIGNED_SIZE)
                {
                    void** deallocation_address = nullptr;
                    
                    if (true_sort_false_constant && _alloc_iterator != nullptr)
                    {
                        for (void** it = (void**)_alloc_iterator; Contains(it); it = (void**)*it)
                        {
                            deallocation_address = it;
                            
                            if (*it > block.ptr)
                            {
                                break;
                            }
                        }
                        
                        NP_ASSERT(deallocation_address != nullptr, "our sorted insert failed");
                    }
                    else
                    {
                        deallocation_address = &_alloc_iterator;
                    }
                    
                    Construct<void*>(block, *deallocation_address);
                    *deallocation_address = block.ptr;
                    block.Invalidate();
                    deallocated = true;
                }
                
                return deallocated;
            }
            
            /**
             deallocates the given block if we contain it
             */
            bl Deallocate(Block& block) override
            {
                return Deallocate(block, false);
            }
            
            /**
             deallocates a block given the ptr and we insert the new free block back using the specified option
             if true we use a sorted insertion based off the block's address to store the new free block
             if false we simply place the new free block as the next block to be allocated - this is constant
             */
            bl Deallocate(void* ptr, bl true_sort_false_constant)
            {
                Block dealloc_block{.ptr = ptr, .size = CHUNK_ALIGNED_SIZE};
                return Deallocate(dealloc_block, true_sort_false_constant);
            }
            
            /**
             deallocates a block given the ptr
             */
            bl Deallocate(void* ptr) override
            {
                Block dealloc_block{.ptr = ptr, .size = CHUNK_ALIGNED_SIZE};
                return Deallocate(dealloc_block);
            }
            
            /**
             deallocates
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

#endif /* NP_ENGINE_LOCKING_POOL_ALLOCATOR_HPP */
