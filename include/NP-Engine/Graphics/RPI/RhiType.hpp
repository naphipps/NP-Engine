//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/22/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RHI_TYPE_HPP
#define NP_ENGINE_RHI_TYPE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::graphics
{
	enum class RhiType
	{
		None,
		Vulkan,
		OpenGL,
		DirectX,
		Metal
	};

	namespace __detail
	{
		extern RhiType RegisteredRhiType;
	}
}

#endif /* NP_ENGINE_RHI_TYPE_HPP */