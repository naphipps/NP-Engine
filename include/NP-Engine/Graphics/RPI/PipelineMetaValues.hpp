//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/25/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_PIPELINE_META_VALUES_HPP
#define NP_ENGINE_RPI_PIPELINE_META_VALUES_HPP

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::gfx
{
	struct PipelineMetaValues
	{
		glm::mat4 View;
		glm::mat4 Projection;
	};
} // namespace np::gfx

#endif /* NP_ENGINE_RPI_PIPELINE_META_VALUES_HPP */
