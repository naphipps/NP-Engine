//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/7/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_SEMAPHORE_HPP
#define NP_ENGINE_GPU_VULKAN_SEMAPHORE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Semaphore.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanSemaphore : public Semaphore
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkSemaphore _semaphore;

		static VkSemaphoreCreateInfo CreateVkInfo()
		{
			VkSemaphoreCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			return info;
		}

		static VkSemaphore CreateVkSemaphore(mem::sptr<VulkanLogicalDevice> device)
		{
			mem::sptr<VulkanInstance> instance = device->GetPhysicalDevice().GetDetailInstance();
			VkSemaphoreCreateInfo info = CreateVkInfo();
			VkSemaphore semaphore = nullptr;
			VkResult result = vkCreateSemaphore(*device, &info, instance->GetVulkanAllocationCallbacks(), &semaphore);
			return result == VK_SUCCESS ? semaphore : nullptr;
		}

	public:
		VulkanSemaphore(mem::sptr<VulkanLogicalDevice> device): _device(device), _semaphore(CreateVkSemaphore(_device)) {}

		~VulkanSemaphore()
		{
			if (_semaphore)
			{
				mem::sptr<VulkanInstance> instance = _device->GetPhysicalDevice().GetDetailInstance();
				vkDestroySemaphore(*_device, _semaphore, instance->GetVulkanAllocationCallbacks());
				_semaphore = nullptr;
			}
		}

		operator VkSemaphore() const
		{
			return _semaphore;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _device;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_SEMAPHORE_HPP */