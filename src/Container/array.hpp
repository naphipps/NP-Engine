//
//  array.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/15/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_ARRAY_HPP
#define NP_ENGINE_ARRAY_HPP

#include <array>

#include "Primitive/Primitive.hpp"

namespace np
{
    namespace container
    {
        /**
         brings the std array here since it is such a small class
         */
        template <class T, siz SIZE>
        using array = ::std::array<T, SIZE>;
    }
}

#endif /* NP_ENGINE_ARRAY_HPP */
