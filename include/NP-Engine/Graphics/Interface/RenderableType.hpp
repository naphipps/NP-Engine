//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/9/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_TYPE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_TYPE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gfx
{
	enum class RenderableType : ui32
	{
		None = 0,
		Model = BIT(0),
		Image = BIT(1),
		Light = BIT(2)
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_TYPE_HPP */