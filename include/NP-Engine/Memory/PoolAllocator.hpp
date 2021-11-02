//
//  PoolAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/15/21.
//

#ifndef NP_ENGINE_POOL_ALLOCATOR_HPP
#define NP_ENGINE_POOL_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "SizedAllocator.hpp"

namespace np
{
    namespace memory
    {
        template <typename T>
        class PoolAllocator : public SizedAllocator
        {
        public:
            constexpr static siz CHUNK_ALIGNED_SIZE = CalcAlignedSize(sizeof(T));
            
        private:
            atm<void*> _alloc_iterator;
            
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
                
                _alloc_iterator.store(_block.ptr, mo_release);
            }
            
        public:
            
            /**
             constructor
             */
            PoolAllocator(Block block):
            SizedAllocator(block)
            {
                Init();
            }
            
            /**
             constructor
             */
            PoolAllocator(siz size):
            SizedAllocator(size)
            {
                Init();
            }
            
            /**
             deconstructor
             */
            ~PoolAllocator() = default;
            
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
             zeroizes our block and initializes the chunks
             */
            void Zeroize() override
            {
                _alloc_iterator.store(nullptr, mo_release);
                SizedAllocator::Zeroize();
                Init();
            }
            
            /**
             allocates a chunk sized block that the given size can fit inside, or returns an invalid block
             */
            Block Allocate(siz size) override
            {
                Block block;
                
                if (size <= CHUNK_ALIGNED_SIZE)
                {
                    void* allocated = _alloc_iterator.load(mo_acquire);
                    
                    while(allocated &&
                          !_alloc_iterator.compare_exchange_weak(allocated, *(void**)(&static_cast<ui8*>(allocated)[0]),
                                                                 mo_release, mo_relaxed));
                    
                    if (allocated)
                    {
                        block.ptr = allocated;
                        block.size = CHUNK_ALIGNED_SIZE;
                    }
                }
                
                return block;
            }
            
            /**
             deallocates the given block if we contain it
             */
            bl Deallocate(Block& block) override
            {
                bl deallocated = false;
                
                if (IsChunkPtr(block.ptr) && block.size == CHUNK_ALIGNED_SIZE)
                {
                    Construct<void*>(block, _alloc_iterator.load(mo_acquire));
                    
                    while(!_alloc_iterator.compare_exchange_weak(*(void**)(&static_cast<ui8*>(block.ptr)[0]), block.ptr,
                                                                 mo_release, mo_relaxed));
                    
                    block.Invalidate();
                    deallocated = true;
                }
                
                return deallocated;
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
                _alloc_iterator.store(nullptr, mo_release);
                Init();
                return true;
            }
        };
    }
}

#endif /* NP_ENGINE_POOL_ALLOCATOR_HPP */
