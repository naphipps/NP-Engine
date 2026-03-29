//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MAT_RANGE_HPP
#define NP_ENGINE_MAT_RANGE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::mat
{
	template <typename T>
	struct range
	{
		union {
			T min = 0;
			T from;
			T a;
			T x;
			T left;
			T begin;
			T start;
		};

		union {
			T max = 0;
			T to;
			T b;
			T y;
			T right;
			T end;
			T last;
		};

		union {
			bl minInclusive = true;
			bl fromInclusive;
			bl aInclusive;
			bl xInclusive;
			bl leftInclusive;
			bl beginInclusive;
			bl startInclusive;
		};

		union {
			bl maxInclusive = true;
			bl toInclusive;
			bl bInclusive;
			bl yInclusive;
			bl rightInclusive;
			bl endInclusive;
			bl lastInclusive;
		};

		range(T min_, T max_, bl min_inclusive_, bl max_inclusive_):
			min(min_),
			max(max_),
			minInclusive(min_inclusive_),
			maxInclusive(max_inclusive_)
		{}

		range(T min_, T max_, bl inclusive_): range(min_, max_, inclusive_, inclusive_) {}

		range(T min_, T max_): range(min_, max_, true) {}

		range() = default;

		virtual bl within(T value) const
		{
			return ((minInclusive && value == min) || value > min) && ((maxInclusive && value == max) || value < max);
		}

		//aliases since we eventually like to use pascal casing
		virtual bl Within(T value) const
		{
			return within(value);
		}
	};

	using range_ui8 = range<ui8>;
	using range_ui16 = range<ui16>;
	using range_ui32 = range<ui32>;
	using range_ui64 = range<ui64>;
	using range_i8 = range<i8>;
	using range_i16 = range<i16>;
	using range_i32 = range<i32>;
	using range_i64 = range<i64>;
	using range_siz = range<siz>;
	using range_flt = range<flt>;
	using range_dbl = range<dbl>;
	using range_ldbl = range<ldbl>;
} //namespace np::mat

#endif /* NP_ENGINE_MAT_RANGE_HPP */