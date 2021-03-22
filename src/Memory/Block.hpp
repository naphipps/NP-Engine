//
//  Block.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/12/21.
//

#ifndef NP_ENGINE_BLOCK_HPP
#define NP_ENGINE_BLOCK_HPP

#include "../Primitive/Primitive.hpp"

namespace np
{
    namespace memory
    {
        /**
         our Block design was inspired by Andrei Alexandrescu's cppcon 2017 talk "std::allocator is to allocation what std::vector is to vexation"
         <https://www.youtube.com/watch?v=LIb3L4vKZ7U>
         */
        struct Block
        {
            void* ptr = nullptr;
            siz size = 0;
            
            /**
             invalidates this block
             */
            void Invalidate()
            {
                ptr = nullptr;
                size = 0;
            }
            
            /**
             checks if this block is valid
             */
            bl IsValid() const
            {
                return ptr != nullptr && size > 0;
            }
            
            /**
             gets the begin pointer of this block
             */
            void* Begin() const
            {
                return IsValid() ? ptr : NULL;
            }
            
            /**
             gets the end pointer of this block
             */
            void* End() const
            {
                return IsValid() ? static_cast<ui8*>(ptr) + size : NULL;
            }
            
            /**
             checks if this block contains the given pointer
             */
            bl Contains(const void* p) const
            {
                return IsValid() ? ptr <= p && p < End() : false;
            }
            
            /**
             checks if this block contains the given block
             */
            bl Contains(const Block& block) const
            {
                return IsValid() && block.IsValid() ?
                Contains(block.Begin()) && Contains(static_cast<ui8*>(block.End())-1) : false;
            }
            
            /**
             zeroizes this block
             */
            void Zeroize()
            {
                for (siz i=0; i<size; i++)
                {
                    static_cast<ui8*>(ptr)[i] = 0;
                }
            }
        };
    }
}

#endif /* NP_ENGINE_BLOCK_HPP */
