//
//  CAllocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/12/21.
//

#ifndef NP_ENGINE_DEFAULT_ALLOCATOR_HPP
#define NP_ENGINE_DEFAULT_ALLOCATOR_HPP

#include <cstdlib> //aligned_alloc and free

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "Allocator.hpp"
#include "Block.hpp"

namespace np
{
    namespace memory
    {
        /**
         DefaultAllocator uses malloc and free to allocate
         */
        class CAllocator : public Allocator
        {
        public:
            
            /**
             constructor
             */
            CAllocator() = default;
            
            /**
             deconstructor
             */
            virtual ~CAllocator() = default;
            
            /**
             check if we contain block
             always true
             */
            virtual bl Contains(const Block& block) const override
            {
                return true;
            }
            
            /**
             check if we contain ptr
             always true
             */
            virtual bl Contains(const void* ptr) const override
            {
                return true;
            }
            
            /**
             allocates a block using new
             */
            virtual Block Allocate(siz size) override
            {
                NP_ASSERT(size > 0, "given size must be greater than 0");
                
                size = CalcAlignedSize(size);
                Block block;
                void* ptr = ::aligned_alloc(ALIGNMENT, size);
                
                if (ptr != nullptr)
                {
                    block.ptr = ptr;
                    block.size = size;
                }
                return block;
            }
            
            /**
             deallocates a block using delete
             */
            virtual bl Deallocate(Block& block) override
            {
                CAllocator::Deallocate(block.ptr);
                block.Invalidate();
                return true;
            }
            
            /**
             deallocates a block given the ptr
             */
            virtual bl Deallocate(void* ptr) override
            {
                ::free(ptr);
                return true;
            }
        };
        
        static CAllocator DefaultAllocator;
    }
}

#endif /* NP_ENGINE_DEFAULT_ALLOCATOR_HPP */
