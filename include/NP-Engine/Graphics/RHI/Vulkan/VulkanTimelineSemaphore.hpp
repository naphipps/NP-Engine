//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/23/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_TIMELINE_SEMAPHORE_HPP
#define NP_ENGINE_VULKAN_TIMELINE_SEMAPHORE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

namespace np::graphics::rhi
{
	class VulkanTimelineSemaphore
	{
	private:
		VkDevice _device;
		VkSemaphore _semaphore;

		VkSemaphore CreateSemaphore(ui64 initial_value)
		{
			VkSemaphore semaphore = nullptr;

			VkSemaphoreTypeCreateInfo type{};
			type.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
			type.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
			type.initialValue = initial_value;

			VkSemaphoreCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			info.pNext = &type;

			if (vkCreateSemaphore(_device, &info, nullptr, &semaphore) != VK_SUCCESS)
				semaphore = nullptr;
			return semaphore;
		}

	public:
		static VkTimelineSemaphoreSubmitInfo CreateTimelineSemaphoreSubmitInfo()
		{
			return {VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO};
		}

		VulkanTimelineSemaphore(VkDevice device, ui64 initial_value = 0):
			_device(device),
			_semaphore(CreateSemaphore(initial_value))
		{
			NP_ENGINE_ASSERT(NP_ENGINE_PLATFORM_SUPPORTS_VULKAN_TIMELINE_SEMAPHORES,
				"Platform must support timeline semaphores to use timeline semaphores.");
		}

		~VulkanTimelineSemaphore()
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

		VkResult Signal(ui64 value)
		{
			VkSemaphoreSignalInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
			info.semaphore = _semaphore;
			info.value = value;
			return vkSignalSemaphore(_device, &info);
		}

		VkResult Wait(ui64 value)
		{
			VkSemaphoreWaitInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
			info.semaphoreCount = 1;
			info.pSemaphores = &_semaphore;
			info.pValues = &value;
			return vkWaitSemaphores(_device, &info, UI64_MAX);
		}

		ui64 GetValue()
		{
			ui64 value = 0;
			vkGetSemaphoreCounterValue(_device, _semaphore, &value);
			return value;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_TIMELINE_SEMAPHORE_HPP */