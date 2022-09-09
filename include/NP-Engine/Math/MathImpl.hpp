//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/16/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MATH_FUNCTIONS_HPP
#define NP_ENGINE_MATH_FUNCTIONS_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "CMathInclude.hpp"

namespace np::mat
{
	/*
		inputs points a, b, and c of a triangle

		if result > 0 then CCW
		else if result < 0 then CW
		else if result == 0 then colinear
	*/
	template <typename T>
	static inline T GetOrientation(const ::glm::vec<2, T>& a, const ::glm::vec<2, T>& b, const ::glm::vec<2, T>& c)
	{
		/*
			|1  1  1 |
			|ax bx cx|
			|ay by cy|

			result = (1/2)((bx*cy - by*cx) - (ax*cy - ay*cx) + (ax*by - ay*bx))
			//but we don't need to divide by 2, just get relationship to 0
		*/

		return b.x * c.y - b.y * c.x - a.x * c.y + a.y * c.x + a.x * b.y - a.y * b.x;
	}

	template <typename T>
	static inline ::glm::vec<2, T> Midpoint(const ::glm::vec<2, T>& a, const ::glm::vec<2, T>& b)
	{
		return {(a.x + b.x) / (T)2, (a.y + b.y) / (T)2};
	}

	template <typename T>
	static inline ::glm::vec<3, T> Midpoint(const ::glm::vec<3, T>& a, const ::glm::vec<3, T>& b)
	{
		return { (a.x + b.x) / (T)2, (a.y + b.y) / (T)2, (a.z + b.z) / (T)2 };
	}

	static inline flt RoundTo32nd(const flt n)
	{
		return ::std::round((dbl)n * 32.0) / 32.0;
	}

	static inline dbl RoundTo32nd(const dbl n)
	{
		return ::std::round(n * 32.0) / 32.0;
	}

	template <typename T>
	static inline T RoundTo32nd(const T n)
	{
		return (T)(::std::round((T)n * 32.0) / 32.0);
	}

	static inline flt RoundTo16th(const flt n)
	{
		return ::std::round((dbl)n * 16.0) / 16.0;
	}

	static inline dbl RoundTo16th(const dbl n)
	{
		return ::std::round(n * 16.0) / 16.0;
	}

	template <typename T>
	static inline T RoundTo16th(const T n)
	{
		return (T)(::std::round((T)n * 16.0) / 16.0);
	}

	static inline i32 FastFloor(const flt n)
	{
		i32 i = static_cast<i32>(n);
		return (n < i) ? (i - 1) : (i);
	}

	static inline i64 FastFloor(const dbl n)
	{
		i64 i = static_cast<i64>(n);
		return (n < i) ? (i - 1) : (i);
	}

	template <typename T>
	static inline T Clamp(const T& n, const T& min, const T& max)
	{
		return n > max ? max : (n < min ? min : n);
	}

	/*
		returns the inverse sqrt of n using Quake III's q_rsqrt algorithm
	*/
	static inline flt InvSqrt(const flt n, i32 newton_iteration_count = 2)
	{
		// keeping union for performance
		union {
			flt f;
			i32 i;
		};

		f = n;
		i = 0x5f3759df - (i >> 1);
		flt half_n = n * 0.5f;

		do
			f *= 1.5f - (half_n * f * f);
		while (--newton_iteration_count > 0);

		return f;
	}

	template <typename T>
	static inline T Pow0(const T n)
	{
		return 1;
	}

	template <typename T>
	static inline T Pow1(const T n)
	{
		return n;
	}

	template <typename T>
	static inline T Pow2(const T n)
	{
		return n * n;
	}

	template <typename T>
	static inline T Pow3(const T n)
	{
		return n * n * n;
	}

	template <typename T>
	static inline T Pow4(const T n)
	{
		return n * n * n * n;
	}

	template <typename T>
	static inline T Pow5(const T n)
	{
		return n * n * n * n * n;
	}

	template <typename T>
	static inline T Pow6(const T n)
	{
		return n * n * n * n * n * n;
	}

	template <typename T>
	static inline T Pow7(const T n)
	{
		return n * n * n * n * n * n * n;
	}

	template <typename T>
	static inline T Pow8(const T n)
	{
		return n * n * n * n * n * n * n * n;
	}

	template <typename T>
	static inline ::glm::vec<2, T> RoundTo32nd(const ::glm::vec<2, T>& point)
	{
		return {RoundTo32nd(point.x), RoundTo32nd(point.y)};
	}

	template <typename T>
	static inline ::glm::vec<2, T> RoundTo16th(const ::glm::vec<2, T>& point)
	{
		return {RoundTo16th(point.x), RoundTo16th(point.y)};
	}

	template <typename T>
	static inline ::glm::vec<3, T> RoundTo32nd(const ::glm::vec<3, T>& point)
	{
		return { RoundTo32nd(point.x), RoundTo32nd(point.y), RoundTo32nd(point.z) };
	}

	template <typename T>
	static inline ::glm::vec<3, T> RoundTo16th(const ::glm::vec<3, T>& point)
	{
		return { RoundTo16th(point.x), RoundTo16th(point.y),  RoundTo16th(point.z) };
	}

	static inline dbl DistanceSquared(const flt ax, const flt ay, const flt bx, const flt by)
	{
		dbl x_diff = ax < bx ? bx - ax : ax - bx;
		dbl y_diff = ay < by ? by - ay : ay - by;
		return Pow2(x_diff) + Pow2(y_diff);
	}

	static inline dbl DistanceSquared(const dbl ax, const dbl ay, const dbl bx, const dbl by)
	{
		dbl x_diff = ax < bx ? bx - ax : ax - bx;
		dbl y_diff = ay < by ? by - ay : ay - by;
		return Pow2(x_diff) + Pow2(y_diff);
	}

	template <typename T>
	static inline ui64 DistanceSquared(const T ax, const T ay, const T bx, const T by)
	{
		ui64 x_diff = ax < bx ? bx - ax : ax - bx;
		ui64 y_diff = ay < by ? by - ay : ay - by;
		return Pow2(x_diff) + Pow2(y_diff);
	}

	static inline dbl DistanceSquared(const ::glm::vec<2, flt>& a, const ::glm::vec<2, flt>& b)
	{
		return DistanceSquared(a.x, a.y, b.x, b.y);
	}

	static inline dbl DistanceSquared(const ::glm::vec<2, dbl>& a, const ::glm::vec<2, dbl>& b)
	{
		return DistanceSquared(a.x, a.y, b.x, b.y);
	}

	template <typename T>
	static inline ui64 DistanceSquared(const ::glm::vec<2, T>& a, const ::glm::vec<2, T>& b)
	{
		return DistanceSquared<T>(a.x, a.y, b.x, b.y);
	}

	static inline dbl DistanceSquared(const flt ax, const flt ay, const flt az, const flt bx, const flt by, const flt bz)
	{
		dbl x_diff = ax < bx ? bx - ax : ax - bx;
		dbl y_diff = ay < by ? by - ay : ay - by;
		dbl z_diff = az < bz ? bz - az : az - bz;
		return Pow2(x_diff) + Pow2(y_diff) + Pow2(z_diff);
	}

	static inline dbl DistanceSquared(const dbl ax, const dbl ay, const dbl az, const dbl bx, const dbl by, const dbl bz)
	{
		dbl x_diff = ax < bx ? bx - ax : ax - bx;
		dbl y_diff = ay < by ? by - ay : ay - by;
		dbl z_diff = az < bz ? bz - az : az - bz;
		return Pow2(x_diff) + Pow2(y_diff) + Pow2(z_diff);
	}

	template <typename T>
	static inline ui64 DistanceSquared(const T ax, const T ay, const T az, const T bx, const T by, const T bz)
	{
		ui64 x_diff = ax < bx ? bx - ax : ax - bx;
		ui64 y_diff = ay < by ? by - ay : ay - by;
		ui64 z_diff = az < bz ? bz - az : az - bz;
		return Pow2(x_diff) + Pow2(y_diff) + Pow2(z_diff);
	}

	static inline dbl DistanceSquared(const ::glm::vec<3, flt>& a, const ::glm::vec<3, flt>& b)
	{
		return DistanceSquared(a.x, a.y, a.z, b.x, b.y, b.z);
	}

	static inline dbl DistanceSquared(const ::glm::vec<3, dbl>& a, const ::glm::vec<3, dbl>& b)
	{
		return DistanceSquared(a.x, a.y, a.z, b.x, b.y, b.z);
	}

	template <typename T>
	static inline ui64 DistanceSquared(const ::glm::vec<3, T>& a, const ::glm::vec<3, T>& b)
	{
		return DistanceSquared<T>(a.x, a.y, a.z, b.x, b.y, b.z);
	}

	static inline flt RoundToInvPowOfTwo(const flt n, const ui8 pow_of_two)
	{
		dbl scalar = ::std::pow(2, pow_of_two);
		return ::std::round((dbl)n * scalar) / scalar;
	}

	static inline dbl RoundToInvPowOfTwo(const dbl n, const ui8 pow_of_two)
	{
		dbl scalar = ::std::pow(2, pow_of_two);
		return ::std::round(n * scalar) / scalar;
	}

	template <typename T>
	static inline T GetAngle(const ::glm::vec<2, T>& point)
	{
		return ::std::atan2(-(point.y), -(point.x));
	}

	static inline dbl GetAngleB(const ::glm::vec<2, flt>& a_pt, const ::glm::vec<2, flt>& b_pt, const ::glm::vec<2, flt>& c_pt)
	{
		dbl a_sq = DistanceSquared(b_pt, c_pt);
		dbl b_sq = DistanceSquared(a_pt, c_pt);
		dbl c_sq = DistanceSquared(b_pt, a_pt);
		dbl a = ::std::sqrt(a_sq);
		dbl c = ::std::sqrt(c_sq);
		dbl B = ::std::acos((a_sq - b_sq + c_sq) / (2.f * a * c));
		return B;
	}

	static inline dbl GetAngleB(const ::glm::vec<2, dbl>& a_pt, const ::glm::vec<2, dbl>& b_pt, const ::glm::vec<2, dbl>& c_pt)
	{
		dbl a_sq = DistanceSquared(b_pt, c_pt);
		dbl b_sq = DistanceSquared(a_pt, c_pt);
		dbl c_sq = DistanceSquared(b_pt, a_pt);
		dbl a = ::std::sqrt(a_sq);
		dbl c = ::std::sqrt(c_sq);
		dbl B = ::std::acos((a_sq - b_sq + c_sq) / (2.f * a * c));
		return B;
	}

	template <typename T>
	static inline ui64 GetAngleB(const ::glm::vec<2, T>& a_pt, const ::glm::vec<2, T>& b_pt, const ::glm::vec<2, T>& c_pt)
	{
		ui64 a_sq = DistanceSquared(b_pt, c_pt);
		ui64 b_sq = DistanceSquared(a_pt, c_pt);
		ui64 c_sq = DistanceSquared(b_pt, a_pt);
		ui64 a = ::std::sqrt(a_sq);
		ui64 c = ::std::sqrt(c_sq);
		ui64 B = ::std::acos((a_sq - b_sq + c_sq) / (2.f * a * c));
		return B;
	}

	template <typename T>
	static inline bl AngleComparerCCW(const ::glm::vec<2, T>& a, const ::glm::vec<2, T>& b)
	{
		return GetAngle(a) < GetAngle(b);
	}

	template <typename T>
	static inline bl AngleComparerCW(const ::glm::vec<2, T>& a, const ::glm::vec<2, T>& b)
	{
		return GetAngle(a) > GetAngle(b);
	}
} // namespace np::mat

#endif /* NP_ENGINE_MATH_FUNCTIONS_HPP */
