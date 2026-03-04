//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/1/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_PIPELINE_CACHE_HPP
#define NP_ENGINE_GPU_INTERFACE_PIPELINE_CACHE_HPP

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

namespace np::gpu
{
	struct PipelineCache
	{
		//static mem::sptr<PipelineCache> Create(con::vector<ui8> initial_data); //TODO: how can we read/write from/to file
		//AND/OR get from our pipelines?

		virtual ~PipelineCache() = default;

		//virtual bl SetBytes(const con::vector<ui8>& bytes) = 0; //TODO: how do we set this?

		virtual con::vector<ui8> GetBytes() const = 0;

		virtual bl Absorb(mem::sptr<PipelineCache> cache) = 0; //TODO: should we put this in Device? I feel like it's fine here?
	};
} //namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_PIPELINE_CACHE_HPP */