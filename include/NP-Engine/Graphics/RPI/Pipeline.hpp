//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_PIPELINE_HPP
#define NP_ENGINE_RPI_PIPELINE_HPP

#include "NP-Engine/Container/Container.hpp"

#include "PipelineMetaValues.hpp"

namespace np::graphics
{
	class Pipeline
	{
	protected:
		container::vector<PipelineMetaValues> _meta_values;

	public:
		virtual PipelineMetaValues GetMetaValues() const = 0;
		virtual void SetMetaValues(PipelineMetaValues meta_values) = 0;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_PIPELINE_HPP */