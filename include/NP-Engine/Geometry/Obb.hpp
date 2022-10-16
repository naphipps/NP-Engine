//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/10/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_OBB_HPP
#define NP_ENGINE_OBB_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Aabb.hpp"

namespace np::geom
{
	template <siz DIMENSION_COUNT, typename T>
	struct Obb;

	template <typename T>
	struct Obb<2, T> : public Aabb<2, T>
	{
		using Point = typename Aabb<2, T>::Point;
		Point Orientation = {0}; // TODO: this should be a quaternion

		virtual bl Contains(const Point& point) const override
		{
			// TODO: implement this
			// turn point into our orientation
			Point local_point = point;
			return Aabb<2, T>::Contains(local_point);
		}
	};

	template <typename T>
	struct Obb<3, T> : public Aabb<3, T>
	{
		using Point = typename Aabb<3, T>::Point;
		Point Orientation = {0}; // TODO: this should be a quaternion

		virtual bl Contains(const Point& point) const override
		{
			// TODO: implement this
			// turn point into our orientation
			Point local_point = point;
			return Aabb<3, T>::Contains(local_point);
		}
	};

	using Ui8Obb2D = Obb<2, ui8>;
	using Ui16Obb2D = Obb<2, ui16>;
	using Ui32Obb2D = Obb<2, ui32>;
	using Ui64Obb2D = Obb<2, ui64>;
	using I8Obb2D = Obb<2, i8>;
	using I16Obb2D = Obb<2, i16>;
	using I32Obb2D = Obb<2, i32>;
	using I64Obb2D = Obb<2, i64>;
	using SizObb2D = Obb<2, siz>;
	using FltObb2D = Obb<2, flt>;
	using DblObb2D = Obb<2, dbl>;
	using LdblObb2D = Obb<2, ldbl>;

	using Ui8Obb3D = Obb<3, ui8>;
	using Ui16Obb3D = Obb<3, ui16>;
	using Ui32Obb3D = Obb<3, ui32>;
	using Ui64Obb3D = Obb<3, ui64>;
	using I8Obb3D = Obb<3, i8>;
	using I16Obb3D = Obb<3, i16>;
	using I32Obb3D = Obb<3, i32>;
	using I64Obb3D = Obb<3, i64>;
	using SizObb3D = Obb<3, siz>;
	using FltObb3D = Obb<3, flt>;
	using DblObb3D = Obb<3, dbl>;
	using LdblObb3D = Obb<3, ldbl>;
} // namespace np::geom

#endif /* NP_ENGINE_OBB_HPP */