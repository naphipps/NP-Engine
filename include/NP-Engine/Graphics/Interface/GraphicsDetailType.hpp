//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/22/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_DETAIL_TYPE_HPP
#define NP_ENGINE_GRAPHICS_DETAIL_TYPE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gfx
{
	enum class GraphicsDetailType : ui32
	{
		None,
		Vulkan,
		OpenGL,
		DirectX,
		Metal
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_DETAIL_TYPE_HPP */