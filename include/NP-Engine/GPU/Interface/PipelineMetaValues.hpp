//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/25/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_PIPELINE_META_VALUES_HPP
#define NP_ENGINE_GPU_INTERFACE_PIPELINE_META_VALUES_HPP

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::gpu
{
	struct PipelineMetaValues
	{
		glm::mat4 View;
		glm::mat4 Projection;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_PIPELINE_META_VALUES_HPP */
