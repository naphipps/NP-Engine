//
//  concurrency_constants.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/6/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_CONCURRENCY_CONSTANTS_HPP
#define NP_ENGINE_CONCURRENCY_CONSTANTS_HPP

#include "Primitive/Primitive.hpp"

namespace np
{
    namespace concurrency
    {
        constexpr static siz CACHE_LINE_SIZE = 64;
        using CacheLinePadding = ui8[CACHE_LINE_SIZE];
    }
}

#endif /* NP_ENGINE_CONCURRENCY_CONSTANTS_HPP */
