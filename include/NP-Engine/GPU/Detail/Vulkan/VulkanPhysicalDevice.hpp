//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 7/26/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_PHYSICAL_DEVICE_HPP
#define NP_ENGINE_GPU_VULKAN_PHYSICAL_DEVICE_HPP

#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanPresentTarget.hpp"
#include "VulkanSampleCount.hpp"

namespace np
{
	namespace gpu::__detail
	{
		class VulkanPhysicalDeviceType : public Enum<ui32>
		{
		public:
			constexpr static ui32 Amd = BIT(0);
			constexpr static ui32 Nvidia = BIT(1);
			constexpr static ui32 Intel = BIT(2);

			VulkanPhysicalDeviceType(ui32 value) : Enum<ui32>(value) {}
		};

		class VulkanPhysicalDevice
		{
		private:
			mem::sptr<VulkanInstance> _instance;
			VkPhysicalDevice _device;

		public:
			static con::vector<str> GetRequiredDeviceExtensionNames()
			{
				con::vector<str> extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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

			static con::vector<VulkanPhysicalDevice> GetAllVulkanPhysicalDevices(mem::sptr<VulkanInstance> instance)
			{
				ui32 count = 0;
				vkEnumeratePhysicalDevices(*instance, &count, nullptr);
				con::vector<VkPhysicalDevice> vulkan_devices(count);
				vkEnumeratePhysicalDevices(*instance, &count, vulkan_devices.data());

				con::vector<VulkanPhysicalDevice> devices;
				for (VkPhysicalDevice vulkan_device : vulkan_devices)
					devices.emplace_back(instance, vulkan_device);
				return devices;
			}

			VulkanPhysicalDevice(mem::sptr<VulkanInstance> instance, VkPhysicalDevice device) :
				_instance(instance),
				_device(device)
			{}

			VulkanPhysicalDevice(const VulkanPhysicalDevice& other) : 
				_instance(other._instance),
				_device(other._device) 
			{}

			VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice& other)
			{
				_instance = other._instance;
				_device = other._device;
				return *this;
			}

			mem::sptr<VulkanInstance> GetDetailInstance() const
			{
				return _instance;
			}

			mem::sptr<srvc::Services> GetServices() const
			{
				return _instance->GetServices();
			}

			ui32 GetUsageScore(DeviceUsage usage, mem::sptr<VulkanPresentTarget> target) const
			{
				//TODO: score physical device based on actual usage
				//TODO: how do we support scoring a physical device based on requiring it to support certain buffer/image formats? VkFormatProperties

				ui32 score = 0;
				VkPhysicalDeviceProperties properties = GetVkProperties();
				VkPhysicalDeviceFeatures features = GetVkFeatures();

				//TODO: consider all the properties and features of the physical device (there are MUCH more than above)

				//device type
				switch (properties.deviceType)
				{
				case VK_PHYSICAL_DEVICE_TYPE_CPU:
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					score += 10;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
					score += 100;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					score += 1000;
					break;
				default:
					break;
				}

				if (features.samplerAnisotropy == VK_TRUE) //TODO: I forget why this is here
					score++;

				// check extensions
				bl supports_required_extensions = false;
				{
					con::vector<str> required_extensions = GetRequiredDeviceExtensionNames();
					if (required_extensions.size() > 0)
					{
						con::uset<str> required_extensions_set(required_extensions.begin(), required_extensions.end());
						con::vector<str> supported_extensions = GetSupportedDeviceExtensionNames();
						for (const str& e : supported_extensions)
							required_extensions_set.erase(e);
						supports_required_extensions = required_extensions_set.empty();
					}
					else
					{
						supports_required_extensions = true;
					}
				}

				// check layers
				bl supports_required_layers = false;
				{
					con::vector<str> required_layers = GetRequiredDeviceLayerNames();
					if (required_layers.size() > 0)
					{
						con::uset<str> required_layers_set(required_layers.begin(), required_layers.end());
						con::vector<str> supported_layers = GetVkSupportedDeviceLayerNames();
						for (const str& l : supported_layers)
							required_layers_set.erase(l);
						supports_required_layers = required_layers_set.empty();
					}
					else
					{
						supports_required_layers = true;
					}
				}

				//check queue families against deevice usage
				const con::vector<VkQueueFamilyProperties> queue_family_properties = GetVkQueueFamilyProperties();

				if (usage.Contains(DeviceUsage::Present) && !HasQueueFamilySupportingPresent(queue_family_properties, target))
					score = 0;

				if (usage.Contains(DeviceUsage::Graphics) && !HasQueueFamilySupportingGraphics(queue_family_properties))
					score = 0;

				if (usage.Contains(DeviceUsage::Compute) && !HasQueueFamilySupportingCompute(queue_family_properties))
					score = 0;

				if (usage.Contains(DeviceUsage::RayTrace))
				{
					//TODO:: what do we check here?
				}

				if (usage.Contains(DeviceUsage::MultiDrawIndirect) && features.multiDrawIndirect == VK_FALSE)
					score = 0;

				if (usage.Contains(DeviceUsage::Tessellation) && features.tessellationShader == VK_FALSE)
					score = 0;

				if (!supports_required_extensions || !supports_required_layers)
					score = 0;
				
				return usage.ContainsAll(DeviceUsage::None) ? 0 : score;
			}

			operator VkPhysicalDevice() const
			{
				return _device;
			}

			VulkanPhysicalDeviceType GetDeviceType() const
			{
				const con::vector<VulkanPhysicalDeviceType> types{ VulkanPhysicalDeviceType::Amd, VulkanPhysicalDeviceType::Nvidia };
				const str name = GetName().toupper();

				VulkanPhysicalDeviceType type = VulkanPhysicalDeviceType::None;
				for (const VulkanPhysicalDeviceType& t : types)
					if (name.find(to_str(t).toupper()) != -1)
					{
						type = t;
						break;
					}
				return type;
			}

			con::vector<VkLayerProperties> GetVkSupportedDeviceLayers() const
			{
				ui32 count;
				vkEnumerateDeviceLayerProperties(_device, &count, nullptr);
				con::vector<VkLayerProperties> layers(count);
				vkEnumerateDeviceLayerProperties(_device, &count, layers.data());
				return layers;
			}

			con::vector<str> GetVkSupportedDeviceLayerNames() const
			{
				con::vector<VkLayerProperties> layers = GetVkSupportedDeviceLayers();
				con::vector<str> names;
				for (VkLayerProperties& layer : layers)
					names.emplace_back(layer.layerName);
				return names;
			}

			con::vector<VkQueueFamilyProperties> GetVkQueueFamilyProperties() const
			{
				ui32 count = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(_device, &count, nullptr);
				con::vector<VkQueueFamilyProperties> properties(count);
				vkGetPhysicalDeviceQueueFamilyProperties(_device, &count, properties.data());
				return properties;
			}

			con::vector<VkExtensionProperties> GetVkSupportedDeviceExtensions() const
			{
				ui32 count = 0;
				vkEnumerateDeviceExtensionProperties(_device, nullptr, &count, nullptr);
				con::vector<VkExtensionProperties> extensions(count);
				vkEnumerateDeviceExtensionProperties(_device, nullptr, &count, extensions.data());
				return extensions;
			}

			con::vector<str> GetSupportedDeviceExtensionNames() const
			{
				con::vector<VkExtensionProperties> extensions = GetVkSupportedDeviceExtensions();
				con::vector<str> names;
				for (VkExtensionProperties& e : extensions)
					names.emplace_back(e.extensionName);
				return names;
			}

			VkPhysicalDeviceProperties GetVkProperties() const
			{
				VkPhysicalDeviceProperties properties{};
				vkGetPhysicalDeviceProperties(_device, &properties);
				return properties;
			}

			::std::pair<VkPhysicalDeviceProperties2, VkPhysicalDeviceVulkan12Properties> GetVk12Properties() const
			{
				VkPhysicalDeviceVulkan12Properties vk12_properties{};
				vk12_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;

				VkPhysicalDeviceProperties2 properties{};
				properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
				properties.pNext = &vk12_properties;

				vkGetPhysicalDeviceProperties2(_device, &properties);
				return { properties, vk12_properties };
			}

			VkPhysicalDeviceProperties2 GetVkProperties2() const
			{
				VkPhysicalDeviceProperties2 properties{};
				properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;

				vkGetPhysicalDeviceProperties2(_device, &properties);
				return properties;
			}

			VkPhysicalDeviceMemoryProperties GetVkMemoryProperties() const
			{
				VkPhysicalDeviceMemoryProperties properties{};
				vkGetPhysicalDeviceMemoryProperties(_device, &properties);
				return properties;
			}

			VkPhysicalDeviceFeatures GetVkFeatures() const
			{
				VkPhysicalDeviceFeatures features{};
				vkGetPhysicalDeviceFeatures(_device, &features);
				return features;
			}

			VkPhysicalDeviceFeatures2 GetVkFeatures2() const
			{
				VkPhysicalDeviceFeatures2 features{};
				features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				vkGetPhysicalDeviceFeatures2(_device, &features);
				return features;
			}

			::std::pair<VkPhysicalDeviceFeatures2, VkPhysicalDeviceVulkan12Features> GetVk12Features() const
			{
				VkPhysicalDeviceVulkan12Features vk12_features{};
				vk12_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;

				VkPhysicalDeviceFeatures2 features{};
				features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features.pNext = &vk12_features;

				vkGetPhysicalDeviceFeatures2(_device, &features);
				return { features, vk12_features };
			}

			VkSurfaceCapabilitiesKHR GetVkSurfaceCapabilities(mem::sptr<VulkanPresentTarget> target) const
			{
				VkSurfaceCapabilitiesKHR capabilities{};
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_device, *target, &capabilities);
				return capabilities;
			}

			con::vector<VkPresentModeKHR> GetVkSurfacePresentModes(mem::sptr<VulkanPresentTarget> target) const
			{
				con::vector<VkPresentModeKHR> modes{};
				if (target)
				{
					ui32 count = 0;
					vkGetPhysicalDeviceSurfacePresentModesKHR(_device, *target, &count, nullptr);
					modes.resize(count);
					vkGetPhysicalDeviceSurfacePresentModesKHR(_device, *target, &count, modes.data());
				}
				return modes;
			}

			con::vector<VkSurfaceFormatKHR> GetVkSurfaceFormats(mem::sptr<VulkanPresentTarget> target) const
			{
				con::vector<VkSurfaceFormatKHR> formats{};
				if (target)
				{
					ui32 count = 0;
					vkGetPhysicalDeviceSurfaceFormatsKHR(_device, *target, &count, nullptr);
					formats.resize(count);
					vkGetPhysicalDeviceSurfaceFormatsKHR(_device, *target, &count, formats.data());
				}
				return formats;
			}

			VkFormatProperties GetVkFormatProperties(VkFormat format) const
			{
				VkFormatProperties properties{};
				vkGetPhysicalDeviceFormatProperties(_device, format, &properties);
				return properties;
			}

			VkImageFormatProperties GetVkImageFormatProperties(VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags) const
			{
				VkImageFormatProperties properties{};
				vkGetPhysicalDeviceImageFormatProperties(_device, format, type, tiling, usage, flags, &properties);
				return properties;
			}

			VkSampleCountFlagBits DetermineVkSampleCountFlagBits(VulkanSampleCount sample_count,
				VkFormat format, VkImageType type, VkImageTiling tiling, VkImageUsageFlags usage, VkImageCreateFlags flags) const
			{
				const VkImageFormatProperties properties = GetVkImageFormatProperties(format, type, tiling, usage, flags);

				mat::SizRange range{ VulkanSampleCount::AllVkSampleCountFlagBitsArray.size() - 1, 0 };
				siz index = 0;

				for (siz i = 0; i < VulkanSampleCount::AllVkSampleCountFlagBitsArray.size(); i++)
				{
					const VkSampleCountFlagBits bits = VulkanSampleCount::AllVkSampleCountFlagBitsArray[i];

					if (properties.sampleCounts & bits)
					{
						range.min = ::std::min(range.min, i);
						range.max = ::std::max(range.max, i);
					}

					if (sample_count.GetVkSampleCountFlagBits() & bits) //this check will only succeed once
						index = i;
				}

				index = ::std::max(index, range.min);
				index = ::std::min(index, range.max);
				return VulkanSampleCount::AllVkSampleCountFlagBitsArray[index];
			}

			::std::optional<ui32> GetMemoryTypeIndex(ui32 type_filter, VkMemoryPropertyFlags flags) const
			{
				const VkPhysicalDeviceMemoryProperties properties = GetVkMemoryProperties();

				::std::optional<ui32> index;
				for (ui32 i = 0; i < properties.memoryTypeCount; i++)
					if ((type_filter & (1 << i)) && (properties.memoryTypes[i].propertyFlags & flags) == flags)
						index = i;

				return index;
			}

			VkFormat GetSupportedVkFormat(const con::vector<VkFormat>& format_candidates, VkImageTiling image_tiling,
				VkFormatFeatureFlags format_features) const
			{
				VkFormat format = VK_FORMAT_UNDEFINED;
				for (const VkFormat& format_candidate : format_candidates)
				{
					VkFormatProperties format_properties = GetVkFormatProperties(format_candidate);

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

			str GetName() const
			{
				return GetVkProperties().deviceName;
			}

			bl QueueFamilySupportsPresent(con::vector<VkQueueFamilyProperties> properties, siz family_index, mem::sptr<VulkanPresentTarget> target) const
			{
				VkBool32 supports = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(_device, family_index, *target, &supports);
				return supports == VK_TRUE;
			}

			bl QueueFamilySupportsGraphics(con::vector<VkQueueFamilyProperties> properties, siz family_index) const
			{
				return family_index < properties.size() && (properties[family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT);
			}

			bl QueueFamilySupportsCompute(con::vector<VkQueueFamilyProperties> properties, siz family_index) const
			{
				return family_index < properties.size() && (properties[family_index].queueFlags & VK_QUEUE_COMPUTE_BIT);
			}

			bl QueueFamilySupportsPresent(siz family_index, mem::sptr<VulkanPresentTarget> target) const
			{
				return QueueFamilySupportsPresent(GetVkQueueFamilyProperties(), family_index, target);
			}

			bl QueueFamilySupportsGraphics(siz family_index) const
			{
				return QueueFamilySupportsGraphics(GetVkQueueFamilyProperties(), family_index);
			}

			bl QueueFamilySupportsCompute(siz family_index) const
			{
				return QueueFamilySupportsCompute(GetVkQueueFamilyProperties(), family_index);
			}

			bl HasQueueFamilySupportingPresent(con::vector<VkQueueFamilyProperties> properties, mem::sptr<VulkanPresentTarget> target) const
			{
				bl found = false;
				for (siz i = 0; !found && i < properties.size(); i++)
					found |= QueueFamilySupportsPresent(i, target);

				return found;
			}

			bl HasQueueFamilySupportingGraphics(con::vector<VkQueueFamilyProperties> properties) const
			{
				bl found = false;
				for (siz i = 0; !found && i < properties.size(); i++)
					found |= properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;

				return found;
			}

			bl HasQueueFamilySupportingCompute(con::vector<VkQueueFamilyProperties> properties) const
			{
				bl found = false;
				for (siz i = 0; !found && i < properties.size(); i++)
					found |= static_cast<bl>(properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT);

				return found;
			}

			bl HasQueueFamilySupportingPresent(mem::sptr<VulkanPresentTarget> target) const
			{
				return HasQueueFamilySupportingPresent(GetVkQueueFamilyProperties(), target);
			}

			bl HasQueueFamilySupportingGraphics() const
			{
				return HasQueueFamilySupportingGraphics(GetVkQueueFamilyProperties());
			}

			bl HasQueueFamilySupportingCompute() const
			{
				return HasQueueFamilySupportingCompute(GetVkQueueFamilyProperties());
			}




			VkSurfaceFormatKHR ChooseVkSurfaceFormat(mem::sptr<VulkanPresentTarget> target) const
			{
				VkSurfaceFormatKHR format{};
				if (target)
				{
					const con::vector<VkSurfaceFormatKHR> formats = GetVkSurfaceFormats(target);
					format = formats.front(); // default if nothing found below

					//TODO: we may need to rank formats to choose best one
					/*
						TODO: VK_COLOR_SPACE_SRGB_NONLINEAR_KHR is the only non-extension value (which is zero)

						TODO: VK_FORMAT_B8G8R8A8_SRGB is convenient for now. We may want to consider Image's desired bit depth?
					*/
					for (const VkSurfaceFormatKHR& f : formats)
						if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
						{
							format = f;
							break;
						}
				}
				return format;
			}

			VkPresentModeKHR ChooseVkPresentMode(mem::sptr<VulkanPresentTarget> target) const
			{
				const con::vector<VkPresentModeKHR> modes = GetVkSurfacePresentModes(target);
				const con::array<VkPresentModeKHR, 3> prefered{ VK_PRESENT_MODE_MAILBOX_KHR , VK_PRESENT_MODE_IMMEDIATE_KHR , VK_PRESENT_MODE_FIFO_KHR };
				// ^ default - if we ever decide to support mobile, then we'll want to use fifo
				// ^ decreasing prefered order, hence the min below

				siz index = prefered.size();
				for (siz i = 0; i < modes.size(); i++)
					for (siz j = 0; j < prefered.size(); j++)
						if (modes[i] == prefered[j])
							index = ::std::min(index, j);

				VkPresentModeKHR mode{};
				if (index < prefered.size())
					mode = prefered[index];
				//TODO: do we want to use a specific value if nothing prefered was found??
				return mode;
			}

			con::vector<VkDisplayPropertiesKHR> GetVkDisplayProperties() const
			{
				ui32 count = 0;
				vkGetPhysicalDeviceDisplayPropertiesKHR(_device, &count, nullptr);
				con::vector<VkDisplayPropertiesKHR> properties(count);
				vkGetPhysicalDeviceDisplayPropertiesKHR(_device, &count, properties.data());
				return properties;
			}

			con::vector<VkDisplayPlanePropertiesKHR> GetVkDisplayPlaneProperties() const
			{
				ui32 count = 0;
				vkGetPhysicalDeviceDisplayPlanePropertiesKHR(_device, &count, nullptr);
				con::vector<VkDisplayPlanePropertiesKHR> properties(count);
				vkGetPhysicalDeviceDisplayPlanePropertiesKHR(_device, &count, properties.data());
				return properties;
			}

			con::vector<VkDisplayModePropertiesKHR> GetVkDisplayModeProperties(VkDisplayKHR display) const
			{
				ui32 count = 0;
				vkGetDisplayModePropertiesKHR(_device, display, &count, nullptr);
				con::vector<VkDisplayModePropertiesKHR> properties(count);
				vkGetDisplayModePropertiesKHR(_device, display, &count, properties.data());
				return properties;
			}
		};
	}// namespace np::gpu::__detail

	static str to_str(const gpu::__detail::VulkanPhysicalDeviceType& type)
	{
		str s = "UNKNOWN";

		switch (type)
		{
		case gpu::__detail::VulkanPhysicalDeviceType::Amd:
			s = "AMD";
			break;

		case gpu::__detail::VulkanPhysicalDeviceType::Nvidia:
			s = "NVIDIA";
			break;

		case gpu::__detail::VulkanPhysicalDeviceType::Intel:
			s = "INTEL";
			break;
		}

		return s;
	}
} // namespace np

#endif /* NP_ENGINE_GPU_VULKAN_PHYSICAL_DEVICE_HPP */