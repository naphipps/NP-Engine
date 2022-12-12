//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/7/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SEMAPHORE_HPP
#define NP_ENGINE_VULKAN_SEMAPHORE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

namespace np::gfx::__detail
{
	class VulkanSemaphore
	{
	private:
		VkDevice _device;
		VkSemaphore _semaphore;

		VkSemaphore CreateSemaphore()
		{
			VkSemaphore semaphore = nullptr;

			VkSemaphoreTypeCreateInfo type{};
			type.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
			type.semaphoreType = VK_SEMAPHORE_TYPE_BINARY;

			VkSemaphoreCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			info.pNext = &type;

			if (vkCreateSemaphore(_device, &info, nullptr, &semaphore) != VK_SUCCESS)
				semaphore = nullptr;
			return semaphore;
		}

	public:
		VulkanSemaphore(VkDevice device): _device(device), _semaphore(CreateSemaphore()) {}

		~VulkanSemaphore()
		{
			if (_semaphore)
			{
				vkDestroySemaphore(_device, _semaphore, nullptr);
				_semaphore = nullptr;
			}
		}

		operator VkSemaphore() const
		{
			return _semaphore;
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_SEMAPHORE_HPP */