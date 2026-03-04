//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/26/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_COLOR_HPP
#define NP_ENGINE_GPU_VULKAN_COLOR_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

namespace np::gpu::__detail
{
	class VulkanColorChannel : public ColorChannel
	{
	public:
		VulkanColorChannel(ui32 value) : ColorChannel(value) {}

		VkColorComponentFlags GetVkColorComponentFlags() const
		{
			VkColorComponentFlags flags = 0;

			if (Contains(R))
				flags |= VK_COLOR_COMPONENT_R_BIT;
			if (Contains(G))
				flags |= VK_COLOR_COMPONENT_G_BIT;
			if (Contains(B))
				flags |= VK_COLOR_COMPONENT_B_BIT;
			if (Contains(A))
				flags |= VK_COLOR_COMPONENT_A_BIT;

			return flags;
		}
	};

	struct VulkanClearColor
	{
		Color color;
		flt depth;
		ui32 stencil;

		VulkanClearColor(const ClearColor& other = {}) :
			color(other.color),
			depth(other.depth),
			stencil(other.stencil)
		{}

		operator ClearColor() const
		{
			return { color, depth, stencil };
		}

		VkClearColorValue GetVkClearColorValue() const
		{
			return
			{
				(flt)color.r / (flt)UI8_MAX,
				(flt)color.g / (flt)UI8_MAX,
				(flt)color.b / (flt)UI8_MAX,
				(flt)color.a / (flt)UI8_MAX
			};
		}

		VkClearDepthStencilValue GetVkClearDepthStencilValue() const
		{
			return { depth, stencil };
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_VULKAN_COLOR_HPP */