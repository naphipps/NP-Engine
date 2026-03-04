//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_VIEWPORT_HPP
#define NP_ENGINE_GPU_INTERFACE_VIEWPORT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "DepthStencil.hpp"

namespace np::gpu
{
	struct Viewport
	{
		::glm::dvec2 position{};
		dbl width = 0;
		dbl height = 0;
		DepthBounds depthBounds{};
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_VIEWPORT_HPP */