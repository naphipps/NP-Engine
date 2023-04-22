//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/19/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RESOURCE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RESOURCE_HPP

#include "ResourceType.hpp"

namespace np::gfx
{
	struct Resource
	{
		virtual ResourceType GetType() const = 0;
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RESOURCE_HPP */