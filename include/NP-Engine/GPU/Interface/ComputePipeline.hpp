//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_COMPUTE_PIPELINE_HPP
#define NP_ENGINE_GPU_INTERFACE_COMPUTE_PIPELINE_HPP

#include "Pipeline.hpp"
#include "Shader.hpp"

namespace np::gpu
{
	struct ComputePipeline : public Pipeline
	{
		static mem::sptr<ComputePipeline> Create(mem::sptr<Shader> shader, mem::sptr<PipelineResourceLayout> layout,
												 PipelineUsage usage, mem::sptr<PipelineCache> cache);

		virtual ~ComputePipeline() = default;

		virtual PipelineType GetPipelineType() const override
		{
			return PipelineType::Compute;
		}

		virtual mem::sptr<Shader> GetShader() const = 0;
	};
} //namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_COMPUTE_PIPELINE_HPP */