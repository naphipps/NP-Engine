//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_CAMERA_HPP
#define NP_ENGINE_RPI_CAMERA_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::graphics
{
	class Camera
	{
	private:
		::glm::mat4 _view{1.0f};
		::glm::mat4 _projection{1.0f};

	public:
		flt Fovy;
		flt AspectRatio;
		flt NearPlane;
		flt FarPlane;

		::glm::vec3 Eye;
		::glm::vec3 Center;
		::glm::vec3 Up{0.0f, 0.0f, 1.0f};

		void Update()
		{
			_view = ::glm::lookAt(Eye, Center, Up);
			_projection = ::glm::perspective(Fovy, AspectRatio, NearPlane, FarPlane);
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
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_CAMERA_HPP */