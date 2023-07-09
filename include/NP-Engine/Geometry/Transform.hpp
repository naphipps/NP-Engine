//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 7/4/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TRANSFORM_HPP
#define NP_ENGINE_TRANSFORM_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::geom
{
	struct Transform
	{
		::glm::vec3 position{0.f}; // default: {0, 0, 0}
		::glm::quat orientation{0.f, 0.f, 0.f, 1.f}; // default: wxyz {0, 0, 0, 1}
		::glm::vec3 scale{1.f}; // default: {1, 1, 1}

		static ::glm::mat4 ToMat4(const Transform& transform) // defined as translate * rotate * scale (trs)
		{
			::glm::mat4 mat{1.0f};
			mat = ::glm::translate(mat, transform.position);
			mat *= ::glm::toMat4(transform.orientation);
			mat = ::glm::scale(mat, transform.scale);
			return mat;
		}

		operator ::glm::mat4()
		{
			return ToMat4(*this);
		}
	};
} // namespace np::geom

#endif /* NP_ENGINE_TRANSFORM_HPP */