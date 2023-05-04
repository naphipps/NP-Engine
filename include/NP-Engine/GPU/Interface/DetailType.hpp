//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/22/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_DETAIL_TYPE_HPP
#define NP_ENGINE_GPU_DETAIL_TYPE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	enum class DetailType : ui32
	{
		None,
		Vulkan,
		OpenGL,
		DirectX,
		Metal
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_DETAIL_TYPE_HPP */