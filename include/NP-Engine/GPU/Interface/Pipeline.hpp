//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_PIPELINE_HPP
#define NP_ENGINE_GPU_INTERFACE_PIPELINE_HPP

#include "NP-Engine/Container/Container.hpp"

#include "PipelineMetaValues.hpp"

namespace np::gpu
{
	class Pipeline
	{
	protected:
		con::vector<PipelineMetaValues> _meta_values;

	public:
		virtual ~Pipeline() = default;

		virtual PipelineMetaValues GetMetaValues() const = 0;

		virtual void SetMetaValues(PipelineMetaValues meta_values) = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_PIPELINE_HPP */