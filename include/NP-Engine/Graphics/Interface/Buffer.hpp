//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_BUFFER_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_BUFFER_HPP

#include "Resource.hpp"

namespace np::gpu
{
	struct Buffer : public Resource
	{
		virtual ResourceType GetType() const
		{
			return ResourceType::Buffer;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_BUFFER_HPP */