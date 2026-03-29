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
	using steady_clock = ::std::chrono::steady_clock;
	using steady_timestamp = ::std::chrono::time_point<steady_clock>;

	template <class R, class P>
	using duration = ::std::chrono::duration<R, P>;

	template <typename R = dbl>
	using picoseconds_template = duration<R, ::std::pico>;

	template <typename R = dbl>
	using nanoseconds_template = duration<R, ::std::nano>;

	template <typename R = dbl>
	using microseconds_template = duration<R, ::std::micro>;

	template <typename R = dbl>
	using milliseconds_template = duration<R, ::std::milli>;

	template <typename R = dbl>
	using seconds_template = duration<R, ::std::ratio<1>>;

	template <typename R = dbl>
	using minutes_template = duration<R, ::std::ratio<60>>;

	template <typename R = dbl>
	using hours_template = duration<R, ::std::ratio<3600>>;

	using picoseconds = picoseconds_template<>;
	using nanoseconds = nanoseconds_template<>;
	using microseconds = microseconds_template<>;
	using milliseconds = milliseconds_template<>;
	using seconds = seconds_template<>;
	using minutes = minutes_template<>;
	using hours = hours_template<>;

	using picoseconds_dbl = picoseconds_template<dbl>;
	using nanoseconds_dbl = nanoseconds_template<dbl>;
	using microseconds_dbl = microseconds_template<dbl>;
	using milliseconds_dbl = milliseconds_template<dbl>;
	using seconds_dbl = seconds_template<dbl>;
	using minutes_dbl = minutes_template<dbl>;
	using hours_dbl = hours_template<dbl>;

	using picoseconds_ui64 = picoseconds_template<ui64>;
	using nanoseconds_ui64 = nanoseconds_template<ui64>;
	using microseconds_ui64 = microseconds_template<ui64>;
	using milliseconds_ui64 = milliseconds_template<ui64>;
	using seconds_ui64 = seconds_template<ui64>;
	using minutes_ui64 = minutes_template<ui64>;
	using hours_ui64 = hours_template<ui64>;

	template <class T, class R, class P>
	constexpr T duration_cast(const duration<R, P>& d)
	{
		return ::std::chrono::duration_cast<T, R, P>(d);
	}
} // namespace np::tim

#endif /* NP_ENGINE_TIME_HPP */
