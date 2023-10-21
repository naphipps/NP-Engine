//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 7/26/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_PHYSICAL_DEVICE_HPP
#define NP_ENGINE_VULKAN_PHYSICAL_DEVICE_HPP

#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanRenderTarget.hpp"

namespace np::gpu::__detail
{
	enum class VulkanPhysicalDeviceType : ui32
	{
		None = 0,
		Amd,
		Nvidia,

		Max
	};

	class VulkanPhysicalDevice
	{
	private:
		VkPhysicalDevice _physical_device;

	public:
		static con::vector<VulkanPhysicalDevice> GetAllDevices(mem::sptr<DetailInstance> instance)
		{
			VulkanInstance& vulkan_instance = (VulkanInstance&)(*instance);
			ui32 count = 0;
			vkEnumeratePhysicalDevices(vulkan_instance, &count, nullptr);
			con::vector<VkPhysicalDevice> vk_devices(count);
			vkEnumeratePhysicalDevices(vulkan_instance, &count, vk_devices.data());

			con::vector<VulkanPhysicalDevice> devices;
			for (VkPhysicalDevice vk_device : vk_devices)
				devices.emplace_back(vk_device);
			return devices;
		}

		VulkanPhysicalDevice(VkPhysicalDevice physical_device = nullptr): _physical_device(physical_device) {}

		VulkanPhysicalDevice(const VulkanPhysicalDevice& other): _physical_device(other._physical_device) {}

		VulkanPhysicalDevice(VulkanPhysicalDevice&& other) noexcept: _physical_device(::std::move(other._physical_device))
		{
			other.Invalidate();
		}

		VulkanPhysicalDevice& operator=(const VulkanPhysicalDevice& other)
		{
			_physical_device = other._physical_device;
			return *this;
		}

		VulkanPhysicalDevice& operator=(VulkanPhysicalDevice&& other) noexcept
		{
			_physical_device = ::std::move(other._physical_device);
			other.Invalidate();
			return *this;
		}

		bl IsValid() const
		{
			return _physical_device;
		}

		void Invalidate()
		{
			_physical_device = nullptr;
		}

		operator bl() const
		{
			return IsValid();
		}

		operator VkPhysicalDevice() const
		{
			return _physical_device;
		}

		VulkanPhysicalDeviceType GetType() const
		{
			VulkanPhysicalDeviceType type = VulkanPhysicalDeviceType::None;
			if (IsValid())
			{
				VkPhysicalDeviceProperties physical_properties;
				vkGetPhysicalDeviceProperties(_physical_device, &physical_properties);

				str device_name = to_str(physical_properties.deviceName);
				for (siz i = 0; i < device_name.size(); i++)
					device_name[i] = ::std::toupper(device_name[i]);

				if (device_name.find("AMD") != -1)
					type = VulkanPhysicalDeviceType::Amd;
				else if (device_name.find("NVIDIA") != -1)
					type = VulkanPhysicalDeviceType::Nvidia;
			}
			return type;
		}

		con::vector<VkLayerProperties> GetSupportedDeviceLayers() const
		{
			con::vector<VkLayerProperties> layers;
			if (IsValid())
			{
				ui32 count;
				vkEnumerateDeviceLayerProperties(_physical_device, &count, nullptr);
				layers.resize(count);
				vkEnumerateDeviceLayerProperties(_physical_device, &count, layers.data());
			}
			return layers;
		}

		con::vector<str> GetSupportedDeviceLayerNames() const
		{
			con::vector<VkLayerProperties> layers = GetSupportedDeviceLayers();
			con::vector<str> names;
			for (VkLayerProperties& layer : layers)
				names.emplace_back(layer.layerName);
			return names;
		}

		con::vector<VkQueueFamilyProperties> GetQueueFamilyProperties() const
		{
			con::vector<VkQueueFamilyProperties> properties;
			if (IsValid())
			{
				ui32 count = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, nullptr);
				properties.resize(count);
				vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, properties.data());
			}
			return properties;
		}

		con::vector<VkExtensionProperties> GetSupportedDeviceExtensions() const
		{
			con::vector<VkExtensionProperties> extensions;
			if (IsValid())
			{
				ui32 count = 0;
				vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &count, nullptr);
				extensions.resize(count);
				vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &count, extensions.data());
			}
			return extensions;
		}

		con::vector<str> GetSupportedDeviceExtensionNames() const
		{
			con::vector<VkExtensionProperties> extensions = GetSupportedDeviceExtensions();
			con::vector<str> names;
			for (VkExtensionProperties& e : extensions)
				names.emplace_back(e.extensionName);
			return names;
		}

		ui32 GetGraphicsFamilyIndex() const
		{
			::std::optional<ui32> graphics_family_index;
			graphics_family_index.reset(); // ensure it is reset
			if (IsValid())
			{
				ui32 count = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, nullptr);
				con::vector<VkQueueFamilyProperties> queue_families(count);
				vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, queue_families.data());

				for (siz i = 0; i < queue_families.size() && !graphics_family_index.has_value(); i++)
					if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
						// TODO: we may want to compare other queue families?
						graphics_family_index = (ui32)i;
			}
			NP_ENGINE_ASSERT(graphics_family_index.has_value(), "we require graphics family index");
			return graphics_family_index.value();
		}

		ui32 GetPresentFamilyIndex(mem::sptr<RenderTarget> target) const
		{
			::std::optional<ui32> present_family_index;
			present_family_index.reset();
			if (IsValid() && target)
			{
				ui32 count = 0;
				vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, nullptr);
				con::vector<VkQueueFamilyProperties> queue_families(count);
				vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &count, queue_families.data());

				VulkanRenderTarget& render_target = (VulkanRenderTarget&)(*target);
				for (siz i = 0; i < queue_families.size() && !present_family_index.has_value(); i++)
				{
					VkBool32 supported = VK_FALSE;
					vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device, i, render_target, &supported);
					if (supported == VK_TRUE)
						present_family_index = (ui32)i;
				}
			}
			NP_ENGINE_ASSERT(present_family_index.has_value(), "we require present family index");
			return present_family_index.value();
		}

		VkSurfaceFormatKHR ChooseSurfaceFormat(mem::sptr<RenderTarget> target) const
		{
			VkSurfaceFormatKHR surface_format{};
			if (IsValid())
			{
				VulkanRenderTarget& render_target = (VulkanRenderTarget&)(*target);

				ui32 count;
				vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, render_target, &count, nullptr);
				con::vector<VkSurfaceFormatKHR> surface_formats(count);
				vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, render_target, &count, surface_formats.data());

				surface_format = surface_formats.front(); // default

				// TODO: may need to rank formats to choose best one
				for (VkSurfaceFormatKHR& f : surface_formats)
					if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					{
						surface_format = f;
						break;
					}
			}
			return surface_format;
		}

		VkPresentModeKHR ChoosePresentMode(mem::sptr<RenderTarget> target) const
		{
			VkPresentModeKHR present_mode{};
			if (IsValid())
			{
				VulkanRenderTarget& render_target = (VulkanRenderTarget&)(*target);

				ui32 count;
				vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_device, render_target, &count, nullptr);
				con::vector<VkPresentModeKHR> present_modes(count);
				vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_device, render_target, &count, present_modes.data());

				// default - if we ever decide to support mobile, then we'll want to use fifo
				present_mode = VK_PRESENT_MODE_FIFO_KHR;

				for (VkPresentModeKHR& p : present_modes)
					if (p == VK_PRESENT_MODE_IMMEDIATE_KHR)
					{
						present_mode = p;
						break;
					}

				for (VkPresentModeKHR& p : present_modes)
					if (p == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						present_mode = p;
						break;
					}
			}
			return present_mode;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_PHYSICAL_DEVICE_HPP */