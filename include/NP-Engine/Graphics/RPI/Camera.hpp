//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_CAMERA_HPP
#define NP_ENGINE_RPI_CAMERA_HPP

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::graphics
{
	struct Camera
	{
		::glm::mat4 Projection{1.0f};
		::glm::mat4 View{1.0f};
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_CAMERA_HPP */