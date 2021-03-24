//
//  Allocator.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/16/21.
//

#ifndef NP_ENGINE_ALLOCATOR_HPP
#define NP_ENGINE_ALLOCATOR_HPP

#include "Primitive/Primitive.hpp"
#include "Utility/Utility.hpp"

#include "Block.hpp"

namespace np
{
    namespace memory
    {
        /**
         constructs the given type using the given args
         */
        template <typename T, typename ... Args>
        constexpr bl Construct(const Block& block, Args&& ... args)
        {
            bl constructed = false;
            
            if (block.IsValid() && block.size >= sizeof(T))
            {
                new (block.ptr) T(utility::Forward<Args>(args)...);
                constructed = true;
            }
            
            return constructed;
        }
        
        /**
         destructs the object given it's pointer
         */
        template <typename T>
        constexpr bl Destruct(const T* t)
        {
            bl destructed = false;
            
            if (t != NULL)
            {
                t->~T();
                destructed = true;
            }
            
            return destructed;
        }
        
        /**
         the alignment our allocators with adhere to
         */
        constexpr static siz ALIGNMENT = BIT(3);
        
        /**
         calculates an aligned size given an arbitary size
         aka: calculates the next multple of ALIGNMENT if given size is not already a multiple of ALIGNMENT
         */
        constexpr static siz CalcAlignedSize(const siz size)
        {
            return ((size + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;
        }
        
        /**
         Allocator represents the interface all our allocators will adhere to
         */
        class Allocator
        {
        public:
            
            /**
             constructor
             */
            Allocator() = default;
            
            /**
             deconstructor
             */
            virtual ~Allocator() = default;
            
            /**
             check if we contain block
             */
            virtual bl Contains(const Block& block) const = 0;
            
            /**
             check if we contain ptr
             */
            virtual bl Contains(const void* ptr) const = 0;
            
            /**
             allocates a Block of given size
             */
            virtual Block Allocate(siz size) = 0;
            
            /**
             deallocates a given Block
             */
            virtual bl Deallocate(Block& block) = 0;
            
            /**
             deallocates a block given the ptr
             */
            virtual bl Deallocate(void* ptr) = 0;
        };
    }
}

#endif /* NP_ENGINE_ALLOCATOR_HPP */
