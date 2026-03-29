//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/28/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MAT_EASE_FUNCTIONS_HPP
#define NP_ENGINE_MAT_EASE_FUNCTIONS_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::mat
{
	static inline flt flip(const flt n)
	{
		return 1.f - n;
	}

	static inline dbl flip(const dbl n)
	{
		return 1.0 - n;
	}

	static inline flt get_t(const flt& a, const flt& b, const flt& p)
	{
		return (p - a) / (b - a);
	}

	static inline dbl get_t(const dbl& a, const dbl& b, const dbl& p)
	{
		return (p - a) / (b - a);
	}

	/*
		gets the t value from given point, from a to b
	*/
	template <typename T>
	static inline T get_t(const ::glm::vec<2, T>& a, const ::glm::vec<2, T>& b, const ::glm::vec<2, T>& point)
	{
		T x_diff = b.x > a.x ? b.x - a.x : a.x - b.x;
		T y_diff = b.y > a.y ? b.y - a.y : a.y - b.y;

		// we'll use the longest length (x or y direction) so our t value is accurate
		return x_diff > y_diff ? (point.x - a.x) / (b.x - a.x) : (point.y - a.y) / (b.y - a.y);
	}

	static inline flt smooth_max(flt a, flt b, flt t)
	{
		return ::std::log(::std::exp(a * t) + ::std::exp(b * t)) / t;
	}

	static inline dbl smooth_max(dbl a, dbl b, dbl t)
	{
		return ::std::log(::std::exp(a * t) + ::std::exp(b * t)) / t;
	}

	static inline flt smooth_min(flt a, flt b, flt t)
	{
		return -smooth_max(-a, -b, t);
	}

	static inline dbl smooth_min(dbl a, dbl b, dbl t)
	{
		return -smooth_max(-a, -b, t);
	}
} // namespace np::mat

#endif /* NP_ENGINE_MAT_EASE_FUNCTIONS_HPP */
