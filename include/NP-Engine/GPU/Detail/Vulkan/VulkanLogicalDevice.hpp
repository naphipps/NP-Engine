//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/21/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_LOGICAL_DEVICE_HPP
#define NP_ENGINE_GPU_VULKAN_LOGICAL_DEVICE_HPP

#include <optional>
#include <algorithm>

#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanInstance.hpp"
#include "VulkanPhysicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanLogicalDevice
	{
	private:
		VulkanPhysicalDevice _physical_device;
		VkDevice _device;

		static VkDeviceCreateInfo CreateDeviceInfo()
		{
			VkDeviceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			return info;
		}

		static VkDevice CreateVkLogicalDevice(VulkanPhysicalDevice physical_device,
											  const con::vector<VkDeviceQueueCreateInfo>& queue_infos_)
		{
			VkDevice logical_device = nullptr;
			if (physical_device)
			{
				VkPhysicalDeviceFeatures physical_features = physical_device.GetVkFeatures();
				VkPhysicalDeviceVulkan12Features physical_vk12_features = physical_device.GetVk12Features().second;

				con::vector<str> enabled_extensions = VulkanPhysicalDevice::GetRequiredDeviceExtensionNames();
				con::vector<const chr*> enabled_extension_names;
				for (const str& extension : enabled_extensions)
					enabled_extension_names.emplace_back(extension.c_str());

				con::vector<str> enabled_layers = VulkanPhysicalDevice::GetRequiredDeviceLayerNames();
				con::vector<const chr*> enabled_layer_names;
				for (const str& layer : enabled_layers)
					enabled_layer_names.emplace_back(layer.c_str());

				con::vector<flt> queue_priorities{1.f};

				con::vector<VkDeviceQueueCreateInfo> queue_infos{queue_infos_.begin(), queue_infos_.end()};
				for (VkDeviceQueueCreateInfo& info : queue_infos)
				{
					info.queueCount = (ui32)queue_priorities.size();
					info.pQueuePriorities = queue_priorities.data();
				}

				VkPhysicalDeviceCoherentMemoryFeaturesAMD enable_coherent_memory_amd{};
				enable_coherent_memory_amd.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD;
				enable_coherent_memory_amd.deviceCoherentMemory = VK_TRUE;

				if (physical_device.GetDeviceType() == VulkanPhysicalDeviceType::Amd)
					physical_vk12_features.pNext = &enable_coherent_memory_amd;

				VkDeviceCreateInfo device_info = CreateDeviceInfo();
				device_info.pNext = &physical_vk12_features;
				device_info.pEnabledFeatures = &physical_features;
				device_info.queueCreateInfoCount = (ui32)queue_infos.size();
				device_info.pQueueCreateInfos = queue_infos.data();
				device_info.enabledExtensionCount = (ui32)enabled_extension_names.size();
				device_info.ppEnabledExtensionNames = enabled_extension_names.data();
				device_info.enabledLayerCount = (ui32)enabled_layer_names.size();
				device_info.ppEnabledLayerNames = enabled_layer_names.data();

				mem::sptr<VulkanInstance> instance = physical_device.GetDetailInstance();
				VkResult result = vkCreateDevice(physical_device, &device_info, instance->GetVulkanAllocationCallbacks(), &logical_device);
				if (result != VK_SUCCESS)
					logical_device = nullptr;
			}
			return logical_device;
		}

	public:
		static VkDeviceQueueCreateInfo CreateVkDeviceQueueInfo()
		{
			VkDeviceQueueCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			return info;
		}

		VulkanLogicalDevice(VulkanPhysicalDevice physical_device, const con::vector<VkDeviceQueueCreateInfo>& queue_infos):
			_physical_device(physical_device),
			_device(CreateVkLogicalDevice(_physical_device, queue_infos))
		{}

		virtual ~VulkanLogicalDevice()
		{
			if (_device)
			{
				mem::sptr<VulkanInstance> instance = _physical_device.GetDetailInstance();
				vkDestroyDevice(_device, instance->GetVulkanAllocationCallbacks());
				_device = nullptr;
			}
		}

		operator VkDevice() const
		{
			return _device;
		}

		mem::sptr<srvc::Services> GetServices() const
		{
			return _physical_device.GetServices();
		}

		VulkanPhysicalDevice GetPhysicalDevice() const
		{
			return _physical_device;
		}

		void WaitUntilIdle() const
		{
			vkDeviceWaitIdle(_device);
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_LOGICAL_DEVICE_HPP */
