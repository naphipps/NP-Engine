//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/4/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_FLAG_HPP
#define NP_ENGINE_GPU_VULKAN_FLAG_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Flag.hpp"

#include "VulkanLogicalDevice.hpp"
#include "VulkanResult.hpp"

namespace np::gpu::__detail
{
	class VulkanFlag: public Flag
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkEvent _event;

		static VkEventCreateInfo CreateVkInfo()
		{
			VkEventCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
			return info;
		}

		static VkEvent CreateVkEvent(mem::sptr<VulkanLogicalDevice> device)
		{
			mem::sptr<VulkanInstance> instance = device->GetPhysicalDevice().GetDetailInstance();
			VkEventCreateInfo info = CreateVkInfo();
			VkEvent event = nullptr;
			VulkanResult result{ vkCreateEvent(*device, &info, instance->GetVulkanAllocationCallbacks(), &event) };
			return result.Contains(VulkanResult::Success) ? event : nullptr;
		}

	public:
		VulkanFlag(mem::sptr<VulkanLogicalDevice> device) : _device(device), _event(CreateVkEvent(_device))
		{
			Reset();
		}

		~VulkanFlag()
		{
			if (_event)
			{
				mem::sptr<VulkanInstance> instance = _device->GetPhysicalDevice().GetDetailInstance();
				vkDestroyEvent(*_device, _event, instance->GetVulkanAllocationCallbacks());
				_event = nullptr;
			}
		}

		operator VkEvent() const
		{
			return _event;
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

		virtual Result Set() override
		{
			return VulkanResult{ vkSetEvent(*GetLogicalDevice(), _event) };
		}

		virtual Result GetStatus() const override
		{
			return VulkanResult{ vkGetEventStatus(*GetLogicalDevice(), _event) };
		}

		virtual Result Reset() override
		{
			return VulkanResult{ vkResetEvent(*GetLogicalDevice(), _event) };
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_FLAG_HPP */