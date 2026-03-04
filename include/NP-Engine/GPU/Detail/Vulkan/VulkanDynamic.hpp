//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_DYNAMIC_HPP
#define NP_ENGINE_GPU_VULKAN_DYNAMIC_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

namespace np::gpu::__detail
{
	class VulkanDynamicUsage : public DynamicUsage
	{
	public:
		VulkanDynamicUsage(ui32 value) : DynamicUsage(value) {}

		con::vector<VkDynamicState> GetVkDynamicStates() const
		{
			con::vector<VkDynamicState> states{};

			if (Contains(Viewport))
				states.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
			if (Contains(Scissor))
				states.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
			if (Contains(LineWidth))
				states.emplace_back(VK_DYNAMIC_STATE_LINE_WIDTH);
			if (Contains(DepthBias))
				states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BIAS);
			if (Contains(BlendConstants))
				states.emplace_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
			if (Contains(DepthBounds))
				states.emplace_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
			if (Contains(StencilCompareMask))
				states.emplace_back(VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK);
			if (Contains(StencilWriteMask))
				states.emplace_back(VK_DYNAMIC_STATE_STENCIL_WRITE_MASK);
			if (Contains(StencilReferenceValue))
				states.emplace_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);

			return states;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_VULKAN_DYNAMIC_HPP */