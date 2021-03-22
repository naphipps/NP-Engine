//
//  vector.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/14/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_VECTOR_HPP
#define NP_ENGINE_VECTOR_HPP

#include <vector>

namespace np
{
    namespace container
    {
        /**
         brings the std vector here since it is so useful
         */
        template <class T>
        using vector = ::std::vector<T>;
    }
}

#endif /* NP_ENGINE_VECTOR_HPP */
