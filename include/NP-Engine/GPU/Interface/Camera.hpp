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
	class Camera
	{
	public:
		enum class ProjectionType : ui32
		{
			Perspective = 0,
			Orthographic = BIT(0)
		};

	private:
		ProjectionType _projection_type = ProjectionType::Perspective;
		::glm::mat4 _view{1.0f};
		::glm::mat4 _projection{1.0f};

	public:
		flt Fovy = 0;
		flt LeftPlane = 0;
		flt AspectRatio = 0;
		flt RightPlane = 0;
		flt BottomPlane = 0;
		flt TopPlane = 0;
		flt NearPlane = 0;
		flt FarPlane = 0;

		union {
			::glm::vec3 Eye{0};
			::glm::vec3 Position;
		};

		union {
			::glm::vec3 Center{0};
			::glm::vec3 LookAt;
		};

		::glm::vec3 Up{0.0f, 0.0f, 1.0f};

		void Update()
		{
			_view = ::glm::lookAt(Position, LookAt, Up);

			switch (_projection_type)
			{
			case ProjectionType::Perspective:
				_projection = ::glm::perspective(Fovy, AspectRatio, NearPlane, FarPlane);
				break;

			case ProjectionType::Orthographic:
				_projection = ::glm::ortho(LeftPlane, RightPlane, BottomPlane, TopPlane, NearPlane, FarPlane);
				break;

			default:
				NP_ENGINE_ASSERT(false, "unknown projection type found");
				break;
			};

			_projection[1][1] *= -1; // ::glm was made for OpenGL, so Y is inverted. We fix/invert Y here.
		}

		const ::glm::mat4& GetView() const
		{
			return _view;
		}

		const ::glm::mat4& GetProjection() const
		{
			return _projection;
		}

		void SetProjectionType(ProjectionType projection_type)
		{
			_projection_type = projection_type;
		}

		ProjectionType GetProjectionType() const
		{
			return _projection_type;
		}

		bl Contains(const VisibleObject& visible_object) const
		{
			// TODO: implement this
			return true;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_CAMERA_HPP */