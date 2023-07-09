//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/19/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RESOURCE_HPP
#define NP_ENGINE_GPU_INTERFACE_RESOURCE_HPP

#include "ResourceType.hpp"

namespace np::gpu
{
	struct Resource
	{
		virtual ResourceType GetType() const = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RESOURCE_HPP */