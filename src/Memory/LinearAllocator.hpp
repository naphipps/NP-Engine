//
//  LinearAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/12/21.
//

#ifndef NP_ENGINE_STACK_ALLOCATOR_HPP
#define NP_ENGINE_STACK_ALLOCATOR_HPP

#include "Primitive/Primitive.hpp"

#include "SizedAllocator.hpp"
#include "Block.hpp"

namespace np
{
    namespace memory
    {
        class LinearAllocator : public SizedAllocator
        {
        private:
            atm<void*> _alloc_iterator;
            
        public:
            
            /**
             constructor
             */
            LinearAllocator(Block block):
            SizedAllocator(block),
            _alloc_iterator(_block.ptr)
            {}
            
            /**
             constructor
             */
            LinearAllocator(siz size):
            SizedAllocator(size),
            _alloc_iterator(_block.ptr)
            {}
            
            /**
             deconstructor
             */
            ~LinearAllocator() = default;
            
            /**
             deallocates all allocation previously made
             */
            void Zeroize() override
            {
                _alloc_iterator.store(nullptr, mo_release);
                SizedAllocator::Zeroize();
                _alloc_iterator.store(_block.ptr, mo_release);
            }
            
            /**
             allocate a block of memory given block size
             */
            Block Allocate(siz size) override
            {
                void* allocated = _alloc_iterator.load(mo_acquire);
                size = CalcAlignedSize(size);
                
                while (allocated && static_cast<ui8*>(allocated) + size <= _block.End() &&
                       !_alloc_iterator.compare_exchange_weak(allocated, static_cast<ui8*>(allocated) + size,
                                                              mo_release, mo_relaxed));
                
                Block block;
                
                if (allocated && static_cast<ui8*>(allocated) + size <= _block.End())
                {
                    block.ptr = allocated;
                    block.size = size;
                }
                
                return block;
            }
            
            /**
             this allocator does not support individual block deallocation
             */
            bl Deallocate(Block& block) override
            {
                return false;
            }
            
            /**
             deallocates a block given the ptr
             */
            bl Deallocate(void* ptr) override
            {
                return false;
            }
            
            /**
             deallocates all allocation previously made
             */
            bl DeallocateAll() override
            {
                _alloc_iterator.store(_block.ptr, mo_release);
                return true;
            }
        };
    }
}

#endif /* NP_ENGINE_STACK_ALLOCATOR_HPP */
