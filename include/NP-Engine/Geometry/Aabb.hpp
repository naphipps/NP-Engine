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
	struct Aabb<2, T> : public Shape<2, T> // TODO: do we need this??
	{
		using Point = typename Shape<2, T>::Point;

		Point center{0};
		Point halfLengths{0};

		virtual bl Contains(const Point& point) const
		{
			return point.x >= center.x - halfLengths.x && point.x <= center.x + halfLengths.x &&
				point.y >= center.y - halfLengths.y && point.y <= center.y + halfLengths.y;
		}

		virtual con::array<Point, 4> GetCorners() const
		{
			con::array<Point, 4> corners;
			for (siz i = 0; i < corners.size(); i++)
				corners[i] = halfLengths;

			corners[1].x *= -1;
			corners[2].x *= -1;
			corners[2].y *= -1;
			corners[3].y *= -1;

			for (siz i = 0; i < corners.size(); i++)
				corners[i] += center;

			return corners;
		}
	};

	template <typename T>
	struct Aabb<3, T> : public Shape<3, T>
	{
		using Point = typename Shape<3, T>::Point;

		Point center{0};
		Point halfLengths{0};

		virtual bl Contains(const Point& point) const
		{
			return point.x >= center.x - halfLengths.x && point.x <= center.x + halfLengths.x &&
				point.y >= center.y - halfLengths.y && point.y <= center.y + halfLengths.y &&
				point.z > center.z - halfLengths.z && point.z <= center.z + halfLengths.z;
		}

		virtual con::array<Point, 8> GetCorners() const
		{
			con::array<Point, 8> corners;
			for (siz i = 0; i < corners.size(); i++)
				corners[i] = halfLengths;

			corners[1].x *= -1;
			corners[2].x *= -1;
			corners[2].y *= -1;
			corners[3].y *= -1;

			siz half_size = corners.size() / 2;
			for (siz i = 0; i < half_size; i++)
			{
				corners[half_size + i] = corners[i];
				corners[half_size + i].z *= -1;
			}

			for (siz i = 0; i < corners.size(); i++)
				corners[i] += center;

			return corners;
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
