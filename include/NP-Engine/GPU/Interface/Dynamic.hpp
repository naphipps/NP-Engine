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
	class DynamicUsage : public enm_ui32
	{
	public:
		constexpr static ui32 Viewport = BIT(0);
		constexpr static ui32 Scissor = BIT(1);
		constexpr static ui32 LineWidth = BIT(2);
		constexpr static ui32 DepthBias = BIT(3);
		constexpr static ui32 BlendConstants = BIT(4);
		constexpr static ui32 DepthBounds = BIT(5);
		constexpr static ui32 StencilCompareMask = BIT(6);
		constexpr static ui32 StencilWriteMask = BIT(7);
		constexpr static ui32 StencilReferenceValue = BIT(8);

		DynamicUsage(ui32 value): enm_ui32(value) {}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_DYNAMIC_HPP */