//
//  Time.hpp
//  Project Space
//
//  Created by Nathan Phipps on 1/5/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_TIME_HPP
#define NP_ENGINE_TIME_HPP

#include <chrono>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np
{
    namespace time
    {
        using Milliseconds = ::std::chrono::milliseconds;
        using Microseconds = ::std::chrono::microseconds;
        
        using SteadyClock = ::std::chrono::steady_clock;
        using SteadyTimestamp = ::std::chrono::time_point<SteadyClock>;
        
        using DurationMilliseconds = ::std::chrono::duration<dbl, ::std::milli>;
        using DurationMicroseconds = ::std::chrono::duration<dbl, ::std::micro>;
    }
}

#endif /* NP_ENGINE_TIME_HPP */
