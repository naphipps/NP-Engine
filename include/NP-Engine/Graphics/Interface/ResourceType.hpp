//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/9/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RESOURCE_TYPE_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RESOURCE_TYPE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gfx
{
	enum class ResourceType : ui32
	{
		None = 0,
		RenderableModel,
		RenderableImage,
		RenderableLight,
		RenderableTexture, //TODO: ??
		Buffer
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RESOURCE_TYPE_HPP */