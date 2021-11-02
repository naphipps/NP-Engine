//
//  fixed_queue.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/18/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_FIXED_QUEUE_HPP
#define NP_ENGINE_FIXED_QUEUE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np
{
    namespace core
    {
        //TODO: WARNING THIS CLASS IS EXPERIMENTAL WITH CURRENT IMPLEMENTATION
        
        template <typename T, siz SIZE>
        class fixed_queue
        {
        private:
            container::array<T, SIZE> _array;
            
            ui32 _front_index;
            ui32 _back_index;
            
            inline T& operator[](ui32 index)
            {
                return _array[index % Capacity()];
            }
            
            inline const T& operator[](ui32 index) const
            {
                return _array[index % Capacity()];
            }
            
        public:
            
            fixed_queue():
            _front_index(0),
            _back_index(0)
            {}
            
            ~fixed_queue()
            {}
            
            ui32 Capacity() const
            {
                return _array.Size();
            }
            
            ui32 Size() const
            {
                //TODO: this seems to just be wrong -- signed bit seems the most off
                return _back_index % Capacity() - _front_index % Capacity();
            }
            
            bl Empty() const
            {
                return Size() < 1;
            }
            
            bl Full() const
            {
                return Size() == Capacity();
            }
            
            bl Front(T& t)
            {
                bl fronted = false;
                if (!Empty())
                {
                    t = (*this)[_front_index];
                    fronted = true;
                }
                return fronted;
            }
            
            bl Pop(T& t)
            {
                bl popped = false;
                if (!Empty())
                {
                    t = (*this)[_front_index++];
                    popped = true;
                }
                return popped;
            }
            
            bl Push(T& item)
            {
                bl pushed = false;
                if (!Full())
                {
                    (*this)[_back_index++] = item;
                    pushed = true;
                }
                return pushed;
            }
            
            bl Push(const T& item)
            {
                bl pushed = false;
                if (!Full())
                {
                    (*this)[_back_index++] = item;
                    pushed = true;
                }
                return pushed;
            }
            
            void Clear()
            {
                _back_index = 0;
                _front_index = 0;
            }
        };
    }
}

#endif /* NP_ENGINE_FIXED_QUEUE_HPP */
