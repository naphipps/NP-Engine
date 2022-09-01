//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/28/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_EASE_FUNCTIONS_HPP
#define NP_ENGINE_EASE_FUNCTIONS_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::mat
{
	static inline flt Flip(const flt n)
	{
		return 1.f - n;
	}

	static inline dbl Flip(const dbl n)
	{
		return 1.0 - n;
	}

	/*
		gets the t value from given point, from a to b
	*/
	template <typename T>
	static inline T GetT(const ::glm::vec<2, T>& a, const ::glm::vec<2, T>& b, const ::glm::vec<2, T>& point)
	{
		T x_diff = b.x > a.x ? b.x - a.x : a.x - b.x;
		T y_diff = b.y > a.y ? b.y - a.y : a.y - b.y;

		// we'll use the longest length (x or y direction) so our t value is accurate
		return x_diff > y_diff ? (point.x - a.x) / (b.x - a.x) : (point.y - a.y) / (b.y - a.y);
	}

	static inline flt SmoothMax(flt a, flt b, flt t)
	{
		return ::std::log(::std::exp(a * t) + ::std::exp(b * t)) / t;
	}

	static inline dbl SmoothMax(dbl a, dbl b, dbl t)
	{
		return ::std::log(::std::exp(a * t) + ::std::exp(b * t)) / t;
	}

	static inline flt SmoothMin(flt a, flt b, flt t)
	{
		return -SmoothMax(-a, -b, t);
	}

	static inline dbl SmoothMin(dbl a, dbl b, dbl t)
	{
		return -SmoothMax(-a, -b, t);
	}
} // namespace np::mat

#endif /* NP_ENGINE_EASE_FUNCTIONS_HPP */
