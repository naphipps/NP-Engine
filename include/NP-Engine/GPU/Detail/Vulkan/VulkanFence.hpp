//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/7/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_FENCE_HPP
#define NP_ENGINE_GPU_VULKAN_FENCE_HPP

#include <utility>

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Fence.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanFence : public Fence
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkFence _fence;

		static VkFenceCreateInfo CreateVkInfo()
		{
			VkFenceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			return info;
		}

		static VkFence CreateVkFence(mem::sptr<VulkanLogicalDevice> device)
		{
			mem::sptr<VulkanInstance> instance = device->GetPhysicalDevice().GetDetailInstance();
			VkFenceCreateInfo info = CreateVkInfo();
			VkFence fence = nullptr;
			VkResult result = vkCreateFence(*device, &info, instance->GetVulkanAllocationCallbacks(), &fence);
			return result == VK_SUCCESS ? fence : nullptr;
		}

	public:
		VulkanFence(mem::sptr<VulkanLogicalDevice> device): _device(device), _fence(CreateVkFence(_device))
		{
			Reset();
		}

		~VulkanFence()
		{
			if (_fence)
			{
				mem::sptr<VulkanInstance> instance = _device->GetPhysicalDevice().GetDetailInstance();
				vkDestroyFence(*_device, _fence, instance->GetVulkanAllocationCallbacks());
				_fence = nullptr;
			}
		}

		operator VkFence() const
		{
			return _fence;
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

		virtual void Wait(siz timeout) const override
		{
			vkWaitForFences(*GetLogicalDevice(), 1, &_fence, VK_TRUE, timeout);
		}

		void Reset() const
		{
			vkResetFences(*GetLogicalDevice(), 1, &_fence);
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_FENCE_HPP */