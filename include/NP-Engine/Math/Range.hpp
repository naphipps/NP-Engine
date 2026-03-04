//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MATH_RANGE_HPP
#define NP_ENGINE_MATH_RANGE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::mat
{
	template <typename T>
	struct Range
	{
		union
		{
			T min = 0;
			T from;
			T a;
			T x;
			T left;
			T begin;
			T start;
		};

		union
		{
			T max = 0;
			T to;
			T b;
			T y;
			T right;
			T end;
			T last;
		};

		union
		{
			bl minInclusive = true;
			bl fromInclusive;
			bl aInclusive;
			bl xInclusive;
			bl leftInclusive;
			bl beginInclusive;
			bl startInclusive;
		};

		union
		{
			bl maxInclusive = true;
			bl toInclusive;
			bl bInclusive;
			bl yInclusive;
			bl rightInclusive;
			bl endInclusive;
			bl lastInclusive;
		};

		Range(T min_, T max_, bl min_inclusive_, bl max_inclusive_) :
			min(min_),
			max(max_),
			minInclusive(min_inclusive_),
			maxInclusive(max_inclusive_)
		{}

		Range(T min_, T max_, bl inclusive_) : Range(min_, max_, inclusive_, inclusive_) {}

		Range(T min_, T max_) : Range(min_, max_, true) {}

		Range() = default;

		virtual bl Within(T value) const
		{
			return ((minInclusive && value == min) || value > min) && ((maxInclusive && value == max) || value < max);
		}
	};

	using Ui8Range = Range<ui8>;
	using Ui16Range = Range<ui16>;
	using Ui32Range = Range<ui32>;
	using Ui64Range = Range<ui64>;
	using I8Range = Range<i8>;
	using I16Range = Range<i16>;
	using I32Range = Range<i32>;
	using I64Range = Range<i64>;
	using SizRange = Range<siz>;
	using FltRange = Range<flt>;
	using DblRange = Range<dbl>;
	using LdblRange = Range<ldbl>;
} // namespace np::gpu

#endif /* NP_ENGINE_MATH_RANGE_HPP */