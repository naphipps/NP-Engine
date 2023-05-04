//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/7/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SEMAPHORE_HPP
#define NP_ENGINE_VULKAN_SEMAPHORE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanSemaphore
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkSemaphore _semaphore;

		VkSemaphore CreateSemaphore()
		{
			VkSemaphore semaphore = nullptr;

			VkSemaphoreCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

			if (vkCreateSemaphore(*_device, &info, nullptr, &semaphore) != VK_SUCCESS)
				semaphore = nullptr;
			return semaphore;
		}

	public:
		VulkanSemaphore(mem::sptr<VulkanLogicalDevice> device): _device(device), _semaphore(CreateSemaphore()) {}

		~VulkanSemaphore()
		{
			if (_semaphore)
			{
				vkDestroySemaphore(*_device, _semaphore, nullptr);
				_semaphore = nullptr;
			}
		}

		operator VkSemaphore() const
		{
			return _semaphore;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_SEMAPHORE_HPP */