//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_LIGHT_HPP
#define NP_ENGINE_GPU_INTERFACE_LIGHT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"
#include "NP-Engine/Geometry/Geometry.hpp"

#include "Color.hpp"

namespace np::gpu
{
	enum class LightType : ui32
	{
		None = 0,
		Omnidirectional,
		Point
	};

	struct Light
	{
		LightType type = LightType::None;
		dbl radius = 0;
		Color color{0};
		geom::Transform transform{};
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_LIGHT_HPP */