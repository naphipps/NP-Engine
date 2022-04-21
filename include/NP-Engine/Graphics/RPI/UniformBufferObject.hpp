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
	// TODO: I don't like the term "UniformBufferObject"... I think we should choose something that better represents what it
	// does
	// TODO: try out RendererMetaValues or PipelineMetaValues - I like the later since they exist in the pipeline
	struct UniformBufferObject
	{
		glm::mat4 View;
		glm::mat4 Projection;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_UNIFORM_BUFFER_OBJECT_HPP */
