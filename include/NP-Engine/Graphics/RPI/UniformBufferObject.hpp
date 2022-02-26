//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/25/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_UNIFORM_BUFFER_OBJECT_HPP
#define NP_ENGINE_RPI_UNIFORM_BUFFER_OBJECT_HPP

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::graphics
{
	struct UniformBufferObject
	{
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Projection;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_UNIFORM_BUFFER_OBJECT_HPP */
