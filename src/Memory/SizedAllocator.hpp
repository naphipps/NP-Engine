//
//  SizedAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/13/21.
//

#ifndef NP_ENGINE_SIZED_ALLOCATOR_HPP
#define NP_ENGINE_SIZED_ALLOCATOR_HPP

#include "Primitive/Primitive.hpp"

#include "CAllocator.hpp"
#include "Block.hpp"

namespace np
{
    namespace memory
    {
        class SizedAllocator : public CAllocator
        {
        protected:
            Block _block;
            const bl _owns_block;
            
        public:
            
            /**
             constructor
             */
            SizedAllocator(Block block):
            CAllocator(),
            _block(block),
            _owns_block(false)
            {}
            
            /**
             constructor
             */
            SizedAllocator(siz size):
            CAllocator(),
            _block(CAllocator::Allocate(size)),
            _owns_block(true)
            {}
            
            /**
             deconstructor
             */
            virtual ~SizedAllocator()
            {
                if (_owns_block)
                {
                    CAllocator::Deallocate(_block);
                }
            }
            
            /**
             checks if we contain the given block
             */
            virtual bl Contains(const Block& block) const override
            {
                return _block.Contains(block);
            }
            
            /**
             checks is we contain the given pointer
             */
            virtual bl Contains(const void* ptr) const override
            {
                return _block.Contains(ptr);
            }
            
            /**
             gets the size of our allocation
             */
            virtual siz Size() const
            {
                return _block.size;
            }
            
            /**
             indicates if we own our block or not
             */
            virtual bl OwnsBlock() const
            {
                return _owns_block;
            }
            
            /**
             zeroizes our block
             */
            virtual void Zeroize()
            {
                _block.Zeroize();
            }
            
            /**
             allocate block of memory
             */
            virtual Block Allocate(siz size) override = 0;
            
            /**
             deallocate block of memory
             */
            virtual bl Deallocate(Block& block) override = 0;
            
            /**
             deallocates a block given the ptr
             */
            virtual bl Deallocate(void* ptr) override = 0;
            
            /**
             deallocates all memory
             */
            virtual bl DeallocateAll() = 0;
        };
        
        constexpr static ui32 KILOBYTE_SIZE = 1000;
        constexpr static ui32 MEGABYTE_SIZE = 1000000;
        constexpr static ui32 GIGABYTE_SIZE = 1000000000;
    }
}

#endif /* NP_ENGINE_SIZED_ALLOCATOR_HPP */
