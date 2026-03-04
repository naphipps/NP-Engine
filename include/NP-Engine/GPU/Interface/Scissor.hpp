//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_SCISSOR_HPP
#define NP_ENGINE_GPU_INTERFACE_SCISSOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

namespace np::gpu
{
	struct Scissor
	{
		::glm::i64vec2 offset{0};
		siz width = 0;
		siz height = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_SCISSOR_HPP */