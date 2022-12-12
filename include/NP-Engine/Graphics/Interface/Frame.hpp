//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_FRAME_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_FRAME_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gfx
{
	struct Frame
	{
		// TODO: do we want a timestamp and timestamp diff from last timestamp? I think so...?

		virtual bl IsValid() const = 0;
		virtual void Invalidate() = 0;
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_FRAME_HPP */