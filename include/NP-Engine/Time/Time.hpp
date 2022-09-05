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
	// TODO: add all the time ratios, like nano seconds, etc

	/*
		using nanoseconds = duration<long long, nano>;
		using microseconds = duration<long long, micro>;
		using milliseconds = duration<long long, milli>;
		using seconds = duration<long long>;
		using minutes = duration<int, ratio<60>>;
		using hours = duration<int, ratio<3600>>;
	*/

	using Nanoseconds = ::std::chrono::nanoseconds;
	using Milliseconds = ::std::chrono::milliseconds;
	using Microseconds = ::std::chrono::microseconds;
	using Minutes = ::std::chrono::minutes;
	using Hours = ::std::chrono::hours;

	using SteadyClock = ::std::chrono::steady_clock;
	using SteadyTimestamp = ::std::chrono::time_point<SteadyClock>;

	template <class R, class P>
	using Duration = ::std::chrono::duration<R, P>;

	template <class T, class R, class P>
	constexpr T DurationCast(const Duration<R, P>& d)
	{
		return ::std::chrono::duration_cast<T, R, P>(d);
	}

	/*
		using atto = ratio<1, 1000000000000000000LL>;
		using femto = ratio<1, 1000000000000000LL>;
		using pico = ratio<1, 1000000000000LL>;
		using nano = ratio<1, 1000000000>;
		using micro = ratio<1, 1000000>;
		using milli = ratio<1, 1000>;
		using centi = ratio<1, 100>;
		using deci = ratio<1, 10>;
		using deca = ratio<10, 1>;
		using hecto = ratio<100, 1>;
		using kilo = ratio<1000, 1>;
		using mega = ratio<1000000, 1>;
		using giga = ratio<1000000000, 1>;
		using tera = ratio<1000000000000LL, 1>;
		using peta = ratio<1000000000000000LL, 1>;
		using exa = ratio<1000000000000000000LL, 1>;
	*/

	using DurationPicoseconds = Duration<dbl, ::std::pico>;
	using DurationNanoseconds = Duration<dbl, ::std::nano>;
	using DurationMicroseconds = Duration<dbl, ::std::micro>;
	using DurationMilliseconds = Duration<dbl, ::std::milli>;
	using DurationSeconds = Duration<dbl, ::std::ratio<1>>;
	using DurationMinutes = Duration<dbl, ::std::ratio<60>>;
	using DurationHours = Duration<dbl, ::std::ratio<3600>>;
} // namespace np::tim

#endif /* NP_ENGINE_TIME_HPP */
