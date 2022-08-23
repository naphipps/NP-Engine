//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_AABB_HPP
#define NP_ENGINE_AABB_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "MathImpl.hpp"

namespace np::mat
{
	template <typename T, ::glm::qualifier Q = ::glm::qualifier::defaultp>
	struct AABB
	{
		::glm::vec<2, T, Q> LowerLeft;
		::glm::vec<2, T, Q> UpperRight;

		bl Contains(::glm::vec<2, T, Q> point) const
		{
			return point.x >= LowerLeft.x && point.x <= UpperRight.x - 1 && point.y >= LowerLeft.y &&
				point.y <= UpperRight.y - 1;
		}

		::glm::vec<2, T, Q> GetCenter() const
		{
			return Midpoint(LowerLeft, UpperRight);
		}
	};

	using fltAABB = AABB<flt>;
	using dblAABB = AABB<dbl>;
} // namespace np::mat

#endif /* NP_ENGINE_AABB_HPP */
