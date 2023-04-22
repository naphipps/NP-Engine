//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/7/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_FENCE_HPP
#define NP_ENGINE_VULKAN_FENCE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

namespace np::gfx::__detail
{
	class VulkanFence
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device; //TODO: should this be VulkanLogicalDevice??
		VkFence _fence;

		static VkFence CreateFence(mem::sptr<VulkanLogicalDevice> device)
		{
			VkFence fence = nullptr;
			VkFenceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

			if (vkCreateFence(*device, &info, nullptr, &fence) != VK_SUCCESS)
				fence = nullptr;

			return fence;
		}

	public:
		VulkanFence(mem::sptr<VulkanLogicalDevice> device): _device(device), _fence(CreateFence(_device))
		{
			Reset();
		}

		~VulkanFence()
		{
			if (_fence)
			{
				vkDestroyFence(*_device, _fence, nullptr);
				_fence = nullptr;
			}
		}

		operator VkFence() const
		{
			return _fence;
		}

		void Wait()
		{
			vkWaitForFences(*_device, 1, &_fence, VK_TRUE, UI64_MAX);
		}

		void Reset()
		{
			vkResetFences(*_device, 1, &_fence);
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_FENCE_HPP */