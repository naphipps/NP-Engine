//
//  container_traits.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 7/28/21.
//

#ifndef NP_ENGINE_CONTAINER_TRAITS_HPP
#define NP_ENGINE_CONTAINER_TRAITS_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::container
{
    template <class T>
    struct deque_traits
    {
        constexpr static siz T_SIZE = sizeof(T);
        constexpr static siz BUFFER_SIZE = BIT(3);
        constexpr static siz BUFFER_BLOCK_SIZE = BUFFER_SIZE * T_SIZE; //our buffers will be 8 * sizeof(T) just like 64bit libstdc++
    };
}

#endif /* NP_ENGINE_CONTAINER_TRAITS_HPP */
