//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/21/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_DEVICE_HPP
#define NP_ENGINE_VULKAN_DEVICE_HPP

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
		mem::sptr<srvc::Services> _services;
		VulkanPhysicalDevice _physical_device;
		VkDevice _device;

		static VkDeviceCreateInfo CreateDeviceInfo()
		{
			VkDeviceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			return info;
		}

		static VkDevice CreateLogicalDevice(VulkanPhysicalDevice physical_device,
											con::vector<VkDeviceQueueCreateInfo> queue_infos)
		{
			VkDevice logical_device = nullptr;
			if (physical_device)
			{
				VkPhysicalDeviceFeatures physical_features{};
				VkPhysicalDeviceVulkan12Features physical_features12{};
				VkPhysicalDeviceFeatures2 physical_features2{};
				physical_features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
				physical_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				physical_features2.pNext = &physical_features12;
				vkGetPhysicalDeviceFeatures(physical_device, &physical_features);
				vkGetPhysicalDeviceFeatures2(physical_device, &physical_features2);

				con::vector<str> enabled_extensions = GetRequiredDeviceExtensionNames();
				con::vector<const chr*> enabled_extension_names;
				for (const str& extension : enabled_extensions)
					enabled_extension_names.emplace_back(extension.c_str());

				con::vector<str> enabled_layers = GetRequiredDeviceLayerNames();
				con::vector<const chr*> enabled_layer_names;
				for (const str& layer : enabled_layers)
					enabled_layer_names.emplace_back(layer.c_str());

				con::vector<flt> queue_priorities{1.f};
				for (VkDeviceQueueCreateInfo& info : queue_infos)
				{
					info.queueCount = (ui32)queue_priorities.size();
					info.pQueuePriorities = queue_priorities.data();
				}

				VkPhysicalDeviceCoherentMemoryFeaturesAMD enable_coherent_memory_amd{};
				enable_coherent_memory_amd.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_COHERENT_MEMORY_FEATURES_AMD;
				enable_coherent_memory_amd.deviceCoherentMemory = VK_TRUE;

				if (physical_device.GetType() == VulkanPhysicalDeviceType::Amd)
					physical_features12.pNext = &enable_coherent_memory_amd;

				VkDeviceCreateInfo device_info = CreateDeviceInfo();
				device_info.pNext = &physical_features12;
				device_info.pEnabledFeatures = &physical_features;
				device_info.queueCreateInfoCount = (ui32)queue_infos.size();
				device_info.pQueueCreateInfos = queue_infos.data();
				device_info.enabledExtensionCount = (ui32)enabled_extension_names.size();
				device_info.ppEnabledExtensionNames = enabled_extension_names.data();
				device_info.enabledLayerCount = (ui32)enabled_layer_names.size();
				device_info.ppEnabledLayerNames = enabled_layer_names.data();

				if (vkCreateDevice(physical_device, &device_info, nullptr, &logical_device) != VK_SUCCESS)
					logical_device = nullptr;
			}
			return logical_device;
		}

	public:
		static con::vector<str> GetRequiredDeviceExtensionNames()
		{
			con::vector<str> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#if NP_ENGINE_PLATFORM_IS_APPLE
			// Apple uses MoltenVK, hence insert VK_KHR_portability_subset
			extensions.emplace_back("VK_KHR_portability_subset");
#endif
			return extensions;
		}

		static con::vector<str> GetRequiredDeviceLayerNames()
		{
			return {};
		}

		VulkanLogicalDevice(mem::sptr<srvc::Services> services, VulkanPhysicalDevice physical_device,
							con::vector<VkDeviceQueueCreateInfo> queue_infos):
			_services(services),
			_physical_device(physical_device),
			_device(CreateLogicalDevice(_physical_device, queue_infos))
		{}

		virtual ~VulkanLogicalDevice()
		{
			if (_device)
			{
				vkDestroyDevice(_device, nullptr);
				_device = nullptr;
			}
		}

		operator VkDevice() const
		{
			return _device;
		}

		mem::sptr<srvc::Services> GetServices() const
		{
			return _services;
		}

		str GetPhysicalDeviceName() const
		{
			str name = "";
			if (_physical_device)
			{
				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(_physical_device, &properties);
				name = properties.deviceName;
			}
			return name;
		}

		VulkanPhysicalDevice GetPhysicalDevice() const
		{
			return _physical_device;
		}

		::std::optional<ui32> GetMemoryTypeIndex(ui32 type_filter, VkMemoryPropertyFlags flags) const
		{
			VkPhysicalDeviceMemoryProperties properties;
			vkGetPhysicalDeviceMemoryProperties(_physical_device, &properties);

			::std::optional<ui32> index;
			for (ui32 i = 0; i < properties.memoryTypeCount; i++)
				if ((type_filter & (1 << i)) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
					index = i;

			return index;
		}

		VkFormat GetSupportedFormat(const con::vector<VkFormat>& format_candidates, VkImageTiling image_tiling,
									VkFormatFeatureFlags format_features) const
		{
			VkFormat format = VK_FORMAT_UNDEFINED;
			for (const VkFormat& format_candidate : format_candidates)
			{
				VkFormatProperties format_properties;
				vkGetPhysicalDeviceFormatProperties(_physical_device, format_candidate, &format_properties);

				bl found = image_tiling == VK_IMAGE_TILING_LINEAR &&
					(format_properties.linearTilingFeatures & format_features) == format_features;

				found |= image_tiling == VK_IMAGE_TILING_OPTIMAL &&
					(format_properties.optimalTilingFeatures & format_features) == format_features;

				if (found)
				{
					format = format_candidate;
					break;
				}
			}
			return format;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_DEVICE_HPP */
