//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/5/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TIME_HPP
#define NP_ENGINE_TIME_HPP

#include <chrono>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::tim
{
	using SteadyClock = ::std::chrono::steady_clock;
	using SteadyTimestamp = ::std::chrono::time_point<SteadyClock>;

	template <class R, class P>
	using Duration = ::std::chrono::duration<R, P>;

	template <typename R = dbl>
	using Picoseconds = Duration<R, ::std::pico>;

	template <typename R = dbl>
	using Nanoseconds = Duration<R, ::std::nano>;

	template <typename R = dbl>
	using Microseconds = Duration<R, ::std::micro>;

	template <typename R = dbl>
	using Milliseconds = Duration<R, ::std::milli>;

	template <typename R = dbl>
	using Seconds = Duration<R, ::std::ratio<1>>;

	template <typename R = dbl>
	using Minutes = Duration<R, ::std::ratio<60>>;

	template <typename R = dbl>
	using Hours = Duration<R, ::std::ratio<3600>>;

	using DblPicoseconds = Picoseconds<dbl>;
	using DblNanoseoncds = Nanoseconds<dbl>;
	using DblMicroseconds = Microseconds<dbl>;
	using DblMilliseconds = Milliseconds<dbl>;
	using DblSeconds = Seconds<dbl>;
	using DblMinutes = Minutes<dbl>;
	using DblHours = Hours<dbl>;

	using Ui64Picoseconds = Picoseconds<ui64>;
	using Ui64Nanoseoncds = Nanoseconds<ui64>;
	using Ui64Microseconds = Microseconds<ui64>;
	using Ui64Milliseconds = Milliseconds<ui64>;
	using Ui64Seconds = Seconds<ui64>;
	using Ui64Minutes = Minutes<ui64>;
	using Ui64Hours = Hours<ui64>;

	template <class T, class R, class P>
	constexpr T DurationCast(const Duration<R, P>& d)
	{
		return ::std::chrono::duration_cast<T, R, P>(d);
	}
} // namespace np::tim

#endif /* NP_ENGINE_TIME_HPP */
