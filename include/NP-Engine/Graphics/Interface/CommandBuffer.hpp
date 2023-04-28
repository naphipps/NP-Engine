//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/9/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_COMMAND_BUFFER_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_COMMAND_BUFFER_HPP

#include "Command.hpp"

namespace np::gfx
{
	class CommandBuffer
	{
	public:

		virtual bl IsValid() const = 0;

		virtual void Invalidate() = 0;

		virtual void Add(Command& command) = 0;
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_COMMAND_BUFFER_HPP */