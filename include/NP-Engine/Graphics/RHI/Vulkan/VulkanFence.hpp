//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/7/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_FENCE_HPP
#define NP_ENGINE_VULKAN_FENCE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

namespace np::gfx::rhi
{
	class VulkanFence
	{
	private:
		VkDevice _device;
		VkFence _fence;

		VkFence CreateFence()
		{
			VkFence fence = nullptr;
			VkFenceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			if (vkCreateFence(_device, &info, nullptr, &fence) != VK_SUCCESS)
				fence = nullptr;

			return fence;
		}

	public:
		VulkanFence(VkDevice device): _device(device), _fence(CreateFence())
		{
			Reset();
		}

		~VulkanFence()
		{
			if (_fence)
			{
				vkDestroyFence(_device, _fence, nullptr);
				_fence = nullptr;
			}
		}

		operator VkFence() const
		{
			return _fence;
		}

		void Wait()
		{
			vkWaitForFences(_device, 1, &_fence, VK_TRUE, UI64_MAX);
		}

		void Reset()
		{
			vkResetFences(_device, 1, &_fence);
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_FENCE_HPP */