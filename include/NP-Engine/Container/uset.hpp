//
//  unordered_set.hpp
//  Project Space
//
//  Created by Nathan Phipps on 12/31/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_USET_HPP
#define NP_ENGINE_USET_HPP

#include <unordered_set>

namespace np
{
    namespace container
    {
        /**
         brings std unordered_set here for it's usefulness
         */
        template<typename T, class H = ::std::hash<T>>
        using uset = ::std::unordered_set<T, H>;
    }
}

#endif /* NP_ENGINE_USET_HPP */
