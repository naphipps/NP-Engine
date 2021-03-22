//
//  oset.hpp
//  Project Space
//
//  Created by Nathan Phipps on 1/6/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_OSET_HPP
#define NP_ENGINE_OSET_HPP

#include <set>

namespace np
{
    namespace container
    {
        /**
         brings std set here for it's usefulness
         */
        template<typename T, class H = ::std::hash<T>>
        using oset = ::std::set<T, H>;
    }
}

#endif /* NP_ENGINE_OSET_HPP */
