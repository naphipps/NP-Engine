//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_AABB_HPP
#define NP_ENGINE_AABB_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::geom
{
	template <siz DIMENSION_COUNT, typename T>
	struct Aabb;

	template <typename T>
	struct Aabb<2, T>
	{
		::glm::vec<2, T> LowerLeft;
		::glm::vec<2, T> UpperRight;

		bl Contains(::glm::vec<2, T> point) const
		{
			return point.x >= LowerLeft.x && point.x <= UpperRight.x - 1 && point.y >= LowerLeft.y &&
				point.y <= UpperRight.y - 1;
		}

		::glm::vec<2, T> GetCenter() const
		{
			return mat::Midpoint(LowerLeft, UpperRight);
		}
	};

	template <typename T>
	struct Aabb<3, T>
	{
		::glm::vec<3, T> LowerLeft;
		::glm::vec<3, T> UpperRight;

		bl Contains(::glm::vec<3, T> point) const
		{
			return point.x >= LowerLeft.x && point.x <= UpperRight.x - 1 && point.y >= LowerLeft.y &&
				point.y <= UpperRight.y - 1 && point.z >= LowerLeft.z && point.z <= UpperRight.z - 1;
		}

		::glm::vec<3, T> GetCenter() const
		{
			return mat::Midpoint(LowerLeft, UpperRight);
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
