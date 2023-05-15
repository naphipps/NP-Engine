//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VENDOR_GLM_INCLUDE_HPP
#define NP_ENGINE_VENDOR_GLM_INCLUDE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace glm
{
	template <typename T, qualifier Q = qualifier::defaultp>
	constexpr static ::np::bl operator<(const vec<2, T, Q>& a, const vec<2, T, Q>& b)
	{
		return a.x != b.x ? a.x < b.x : a.y < b.y;
	}

	template <typename T, qualifier Q = qualifier::defaultp>
	constexpr static ::np::bl operator<=(const vec<2, T, Q>& a, const vec<2, T, Q>& b)
	{
		return a == b || a < b;
	}

	template <typename T, qualifier Q = qualifier::defaultp>
	constexpr static ::np::bl operator>(const vec<2, T, Q>& a, const vec<2, T, Q>& b)
	{
		return !(a <= b);
	}

	template <typename T, qualifier Q = qualifier::defaultp>
	constexpr static ::np::bl operator>=(const vec<2, T, Q>& a, const vec<2, T, Q>& b)
	{
		return !(a < b);
	}
} // namespace glm

#endif /* NP_ENGINE_VENDOR_GLM_INCLUDE_HPP */