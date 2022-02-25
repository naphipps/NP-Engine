//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/5/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TIME_HPP
#define NP_ENGINE_TIME_HPP

#include <chrono>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::time
{
	using Milliseconds = ::std::chrono::milliseconds;
	using Microseconds = ::std::chrono::microseconds;

	using SteadyClock = ::std::chrono::steady_clock;
	using SteadyTimestamp = ::std::chrono::time_point<SteadyClock>;

	using DurationMilliseconds = ::std::chrono::duration<dbl, ::std::milli>;
	using DurationMicroseconds = ::std::chrono::duration<dbl, ::std::micro>;
} // namespace np::time

#endif /* NP_ENGINE_TIME_HPP */
