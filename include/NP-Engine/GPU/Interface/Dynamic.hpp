//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_DYNAMIC_HPP
#define NP_ENGINE_GPU_INTERFACE_DYNAMIC_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::gpu
{
	class DynamicUsage : public Enum<ui32>
	{
	public:
		constexpr static ui32 Viewport = BIT(0);
		constexpr static ui32 Scissor = BIT(0);
		constexpr static ui32 LineWidth = BIT(0);
		constexpr static ui32 DepthBias = BIT(0);
		constexpr static ui32 BlendConstants = BIT(0);
		constexpr static ui32 DepthBounds = BIT(0);
		constexpr static ui32 StencilCompareMask = BIT(0);
		constexpr static ui32 StencilWriteMask = BIT(0);
		constexpr static ui32 StencilReferenceValue = BIT(0);

		DynamicUsage(ui32 value) : Enum<ui32>(value) {}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_DYNAMIC_HPP */