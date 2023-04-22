//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_COMMAND_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_COMMAND_HPP

#include "GraphicsDetailType.hpp"

namespace np::gfx
{
	struct Command
	{
		virtual GraphicsDetailType GetDetailType() const = 0;
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_COMMAND_HPP */