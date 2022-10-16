//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/27/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CIRCLE_HPP
#define NP_ENGINE_CIRCLE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "Shape.hpp"

namespace np::geom
{
	template <siz DIMENSION_COUNT, typename T>
	struct Circular : public Shape<DIMENSION_COUNT, T>
	{
		using Point = typename Shape<DIMENSION_COUNT, T>::Point;

		Point Center = {};
		T Radius = 0;

		virtual bl IntersectsWith(const Circular<DIMENSION_COUNT, T>& other) const
		{
			return mat::DistanceSquared(Center, other.Center) <= mat::Pow2(Radius + other.Radius);
		}
	};

	template <typename T>
	using Circle = Circular<2, T>;

	template <typename T>
	using Sphere = Circular<3, T>;

	using Ui8Circle = Circle<ui8>;
	using Ui16Circle = Circle<ui16>;
	using Ui32Circle = Circle<ui32>;
	using Ui64Circle = Circle<ui64>;
	using I8Circle = Circle<i8>;
	using I16Circle = Circle<i16>;
	using I32Circle = Circle<i32>;
	using I64Circle = Circle<i64>;
	using SizCircle = Circle<siz>;
	using FltCircle = Circle<flt>;
	using DblCircle = Circle<dbl>;
	using LdblCircle = Circle<ldbl>;

	using Ui8Sphere = Sphere<ui8>;
	using Ui16Sphere = Sphere<ui16>;
	using Ui32Sphere = Sphere<ui32>;
	using Ui64Sphere = Sphere<ui64>;
	using I8Sphere = Sphere<i8>;
	using I16Sphere = Sphere<i16>;
	using I32Sphere = Sphere<i32>;
	using I64Sphere = Sphere<i64>;
	using SizSphere = Sphere<siz>;
	using FltSphere = Sphere<flt>;
	using DblSphere = Sphere<dbl>;
	using LdblSphere = Sphere<ldbl>;
} // namespace np::geom

#endif /* NP_ENGINE_CIRCLE_HPP */