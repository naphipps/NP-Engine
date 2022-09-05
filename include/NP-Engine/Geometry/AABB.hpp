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
	// TODO: make our AABB support 3D points

	template <typename T>
	struct AABB
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

	using ui8AABB = AABB<ui8>;
	using ui16AABB = AABB<ui16>;
	using ui32AABB = AABB<ui32>;
	using ui64AABB = AABB<ui64>;
	using i8AABB = AABB<i8>;
	using i16AABB = AABB<i16>;
	using i32AABB = AABB<i32>;
	using i64AABB = AABB<i64>;
	using sizAABB = AABB<siz>;
	using fltAABB = AABB<flt>;
	using dblAABB = AABB<dbl>;
	using ldblAABB = AABB<ldbl>;
} // namespace np::geom

#endif /* NP_ENGINE_AABB_HPP */
