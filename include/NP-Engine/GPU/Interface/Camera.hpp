//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_CAMERA_HPP
#define NP_ENGINE_GPU_INTERFACE_CAMERA_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "VisibleObject.hpp"

namespace np::gpu
{
	struct Camera
	{
		enum class ProjectionType : ui32
		{
			Perspective = 0,
			Orthographic = BIT(0)
		};

		constexpr static ::glm::vec3 Up{ 0.f, 1.f, 0.f };
		constexpr static ::glm::vec3 Forward{ 0.f, 0.f, 1.f };

		ProjectionType projectionType = ProjectionType::Perspective;
		::glm::mat4 view{1.0f};
		::glm::mat4 projection{1.0f};

		flt fovy = 0;
		flt leftPlane = 0;
		flt aspectRatio = 0;
		flt rightPlane = 0;
		flt bottomPlane = 0;
		flt topPlane = 0;
		flt nearPlane = 0;
		flt farPlane = 0;

		union {
			::glm::vec3 eye{0};
			::glm::vec3 position;
		};

		union {
			::glm::vec3 lookAt{0};
			::glm::vec3 target;
			::glm::vec3 center;
		};

		void UpdateViewAndProjection()
		{
			view = ::glm::lookAt(position, lookAt, Up);

			switch (projectionType)
			{
			case ProjectionType::Perspective:
				projection = ::glm::perspective(fovy, aspectRatio, nearPlane, farPlane);
				break;

			case ProjectionType::Orthographic:
				projection = ::glm::ortho(leftPlane, rightPlane, bottomPlane, topPlane, nearPlane, farPlane);
				break;

			default:
				NP_ENGINE_ASSERT(false, "unknown projection type found");
				break;
			};

			projection[1][1] *= -1; // ::glm was made for OpenGL, so Y is inverted. We fix/invert Y here.
		}

		void NormalizeLookAt()
		{
			lookAt = eye + ::glm::normalize(lookAt - eye);
		}

		::glm::vec3 GetLookDirection() const
		{
			return lookAt - eye;
		}

		bl _contains = true;
		bl Contains(const VisibleObject& visible_object) const
		{
			// TODO: implement this
			return _contains;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_CAMERA_HPP */