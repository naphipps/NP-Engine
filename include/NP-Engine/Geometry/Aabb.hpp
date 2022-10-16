//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_AABB_HPP
#define NP_ENGINE_AABB_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Shape.hpp"

namespace np::geom
{
	template <siz DIMENSION_COUNT, typename T>
	struct Aabb;

	template <typename T>
	struct Aabb<2, T> : public Shape<2, T>
	{
		using Point = typename Shape<2, T>::Point;

		Point Center = {0};
		Point HalfLengths = {0};

		virtual bl Contains(const Point& point) const
		{
			return point.x >= Center.x - HalfLengths.x && point.x <= Center.x + HalfLengths.x &&
				point.y >= Center.y - HalfLengths.y && point.y <= Center.y + HalfLengths.y;
		}
	};

	template <typename T>
	struct Aabb<3, T> : public Shape<3, T>
	{
		using Point = typename Shape<3, T>::Point;

		Point Center = {0};
		Point HalfLengths = {0};

		virtual bl Contains(const Point& point) const
		{
			return point.x >= Center.x - HalfLengths.x && point.x <= Center.x + HalfLengths.x &&
				point.y >= Center.y - HalfLengths.y && point.y <= Center.y + HalfLengths.y &&
				point.z > Center.z - HalfLengths.z && point.z <= Center.z + HalfLengths.z;
		}
	};

	using Ui8Aabb2D = Aabb<2, ui8>;
	using Ui16Aabb2D = Aabb<2, ui16>;
	using Ui32Aabb2D = Aabb<2, ui32>;
	using Ui64Aabb2D = Aabb<2, ui64>;
	using I8Aabb2D = Aabb<2, i8>;
	using I16Aabb2D = Aabb<2, i16>;
	using I32Aabb2D = Aabb<2, i32>;
	using I64Aabb2D = Aabb<2, i64>;
	using SizAabb2D = Aabb<2, siz>;
	using FltAabb2D = Aabb<2, flt>;
	using DblAabb2D = Aabb<2, dbl>;
	using LdblAabb2D = Aabb<2, ldbl>;

	using Ui8Aabb3D = Aabb<3, ui8>;
	using Ui16Aabb3D = Aabb<3, ui16>;
	using Ui32Aabb3D = Aabb<3, ui32>;
	using Ui64Aabb3D = Aabb<3, ui64>;
	using I8Aabb3D = Aabb<3, i8>;
	using I16Aabb3D = Aabb<3, i16>;
	using I32Aabb3D = Aabb<3, i32>;
	using I64Aabb3D = Aabb<3, i64>;
	using SizAabb3D = Aabb<3, siz>;
	using FltAabb3D = Aabb<3, flt>;
	using DblAabb3D = Aabb<3, dbl>;
	using LdblAabb3D = Aabb<3, ldbl>;
} // namespace np::geom

#endif /* NP_ENGINE_AABB_HPP */
