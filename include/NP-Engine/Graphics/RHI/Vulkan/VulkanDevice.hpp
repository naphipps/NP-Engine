//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/21/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_DEVICE_HPP
#define NP_ENGINE_VULKAN_DEVICE_HPP

#include <iostream> //TODO: remove
#include <optional>
#include <algorithm>

#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"
#include "NP-Engine/Vendor/GlfwInclude.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"

namespace np::graphics::rhi
{
	class VulkanDevice
	{
	public:
		struct QueueFamilyIndices_T
		{
			::std::optional<ui32> graphics;
			::std::optional<ui32> present;

			void clear()
			{
				graphics.reset();
				present.reset();
			}

			bl is_complete() const
			{
				return graphics.has_value() && present.has_value();
			}

			container::vector<ui32> to_vector() const
			{
				container::vector<ui32> v;

				if (graphics.has_value() && present.has_value())
				{
					v = {graphics.value(), present.value()};
				}

				return v;
			}
		};

	private:
		VulkanSurface& _surface;
		QueueFamilyIndices_T _queue_family_indices;
		VkSurfaceCapabilitiesKHR _surface_capabilities;
		VkSurfaceFormatKHR _surface_format;
		VkPresentModeKHR _present_mode;
		VkPhysicalDevice _physical_device;
		VkDevice _device;

		container::vector<VkExtensionProperties> GetSupportedDeviceExtensions(VkPhysicalDevice physical_device) const
		{
			ui32 count = 0;
			vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count, nullptr);
			container::vector<VkExtensionProperties> extensions(count);
			vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &count, extensions.data());
			return extensions;
		}

		container::vector<str> GetSupportedDeviceExtensionNames(VkPhysicalDevice physical_device) const
		{
			container::vector<VkExtensionProperties> extensions = GetSupportedDeviceExtensions(physical_device);
			container::vector<str> names;
			for (auto e : extensions)
				names.emplace_back(e.extensionName);
			return names;
		}

		container::vector<VkLayerProperties> GetSupportedDeviceLayers(VkPhysicalDevice physical_device) const
		{
			ui32 count;
			vkEnumerateDeviceLayerProperties(physical_device, &count, nullptr);
			container::vector<VkLayerProperties> layers(count);
			vkEnumerateDeviceLayerProperties(physical_device, &count, layers.data());
			return layers;
		}

		container::vector<str> GetSupportedDeviceLayerNames(VkPhysicalDevice physical_device) const
		{
			container::vector<VkLayerProperties> layers = GetSupportedDeviceLayers(physical_device);
			container::vector<str> names;
			for (auto e : layers) // TODO: can these be references?
				names.emplace_back(e.layerName);
			return names;
		}

		container::vector<VkPhysicalDevice> GetPhysicalDevices() const
		{
			ui32 count = 0;
			vkEnumeratePhysicalDevices(Surface().Instance(), &count, nullptr);
			container::vector<VkPhysicalDevice> devices(count);
			vkEnumeratePhysicalDevices(Surface().Instance(), &count, devices.data());
			return devices;
		}

		container::vector<VkQueueFamilyProperties> GetQueueFamilyProperties(VkPhysicalDevice device) const
		{
			ui32 count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
			container::vector<VkQueueFamilyProperties> properties(count);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());
			return properties;
		}

		container::vector<str> GetRequiredDeviceExtensionNames() const
		{
			container::vector<str> extensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

#if NP_ENGINE_PLATFORM_IS_APPLE
			// Apple uses MoltenVK, hence insert VK_KHR_portability_subset
			extensions.emplace_back("VK_KHR_portability_subset");
#endif
			return extensions;
		}

		container::vector<str> GetRequiredDeviceLayerNames() const
		{
			return {};
		}

		ui32 GetPhysicalDeviceScore(VkPhysicalDevice physical_device)
		{
			ui32 score = 0;

			if (physical_device != nullptr && (VkSurfaceKHR)Surface() != nullptr)
			{
				VkPhysicalDeviceProperties properties{};
				VkPhysicalDeviceFeatures features{};
				vkGetPhysicalDeviceProperties(physical_device, &properties);
				vkGetPhysicalDeviceFeatures(physical_device, &features);

				switch (properties.deviceType)
				{
				case VK_PHYSICAL_DEVICE_TYPE_CPU:
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					score += 10;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					score += 1000;
					break;
				case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
					score += 100;
					break;
				default:
					break;
				}

				// TODO: go through all properties and features to determine best score...

				bl has_queuegraphics_bit = false; // we require the VK_QUEUEgraphics_BIT
				bl has_queue_present_bit = false; // we require KHR present support
				bl supports_required_extensions = false;
				bl supports_required_layers = false;

				// check queue families
				{
					container::vector<VkQueueFamilyProperties> queue_family_properties =
						GetQueueFamilyProperties(physical_device);

					for (siz i = 0; i < queue_family_properties.size(); i++)
					{
						if (!has_queuegraphics_bit && (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
						{
							has_queuegraphics_bit = true;
						}

						if (!has_queue_present_bit)
						{
							VkBool32 supported = false;
							vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, Surface(), &supported);
							has_queue_present_bit = (bl)supported;
						}
					}
				}

				// check extensions
				{
					container::vector<str> required_extensions = GetRequiredDeviceExtensionNames();
					if (required_extensions.size() > 0)
					{
						container::uset<str> required_extensions_set(required_extensions.begin(), required_extensions.end());
						container::vector<str> supported_extensions = GetSupportedDeviceExtensionNames(physical_device);
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
				{
					container::vector<str> required_layers = GetRequiredDeviceLayerNames();
					if (required_layers.size() > 0)
					{
						container::uset<str> required_layers_set(required_layers.begin(), required_layers.end());
						container::vector<str> supported_layers = GetSupportedDeviceLayerNames(physical_device);
						for (const str& l : supported_layers)
							required_layers_set.erase(l);
						supports_required_layers = required_layers_set.empty();
					}
					else
					{
						supports_required_layers = true;
					}
				}

				if (!has_queuegraphics_bit || !has_queue_present_bit || !supports_required_extensions ||
					!supports_required_layers)
				{
					score = 0;
				}
			}

			return score;
		}

		container::vector<VkDeviceQueueCreateInfo> CreateQueueCreateInfos()
		{
			container::vector<VkDeviceQueueCreateInfo> infos;

			if (_physical_device != nullptr)
			{
				container::oset<ui32> families;
				families.emplace(_queue_family_indices.graphics.value());
				families.emplace(_queue_family_indices.present.value());

				for (ui32 family : families)
				{
					VkDeviceQueueCreateInfo info{};
					info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					info.queueFamilyIndex = family;
					infos.emplace_back(info);
				}
			}

			return infos;
		}

		VkDeviceCreateInfo CreateDeviceInfo()
		{
			VkDeviceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			return info;
		}

		void ChooseSurfaceFormat(container::vector<VkSurfaceFormatKHR>& surface_formats)
		{
			_surface_format = {};

			if (_physical_device != nullptr)
			{
				_surface_format = surface_formats[0];

				// TODO: may need to rank formats to choose best one
				for (VkSurfaceFormatKHR& f : surface_formats)
				{
					if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					{
						_surface_format = f;
						break;
					}
				}
			}
		}

		void ChoosePresentMode(container::vector<VkPresentModeKHR>& present_modes)
		{
			_present_mode = {};

			if (_physical_device != nullptr)
			{
				_present_mode = VK_PRESENT_MODE_FIFO_KHR;

				// TODO: if we ever decide to support mobile, then we'll want to use fifo
				for (VkPresentModeKHR& p : present_modes)
				{
					if (p == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						_present_mode = p;
						break;
					}
				}
			}
		}

		VkPhysicalDevice ChoosePhysicalDevice()
		{
			VkPhysicalDevice physical_device = nullptr;
			container::vector<VkSurfaceFormatKHR> surface_formats;
			container::vector<VkPresentModeKHR> present_modes;
			container::ommap<i32, VkPhysicalDevice> candidates;
			for (VkPhysicalDevice device : GetPhysicalDevices())
				candidates.emplace(GetPhysicalDeviceScore(device), device);

			if ((VkSurfaceKHR)Surface() != nullptr && candidates.size() > 0)
			{
				ui32 count = 0;
				container::vector<VkQueueFamilyProperties> queue_families;
				for (auto it = candidates.rbegin(); it->first > 0 && it != candidates.rend(); it++)
				{
					// first = score, second = physical device
					vkGetPhysicalDeviceQueueFamilyProperties(it->second, &count, nullptr);
					// queue_families.clear(); //this might or might not be needed
					queue_families.resize(count);
					vkGetPhysicalDeviceQueueFamilyProperties(it->second, &count, queue_families.data());

					_queue_family_indices.clear();
					for (siz i = 0; i < queue_families.size(); i++)
					{
						if (!_queue_family_indices.graphics.has_value() && queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
						{
							_queue_family_indices.graphics = (ui32)i;
						}

						if (!_queue_family_indices.present.has_value())
						{
							VkBool32 supported = VK_FALSE;
							vkGetPhysicalDeviceSurfaceSupportKHR(it->second, i, Surface(), &supported);
							if (supported == VK_TRUE)
								_queue_family_indices.present = (ui32)i;
						}
					}

					vkGetPhysicalDeviceSurfaceCapabilitiesKHR(it->second, Surface(), &_surface_capabilities);

					ui32 count;
					vkGetPhysicalDeviceSurfaceFormatsKHR(it->second, Surface(), &count, nullptr);
					surface_formats.resize(count);
					vkGetPhysicalDeviceSurfaceFormatsKHR(it->second, Surface(), &count, surface_formats.data());

					vkGetPhysicalDeviceSurfacePresentModesKHR(it->second, Surface(), &count, nullptr);
					present_modes.resize(count);
					vkGetPhysicalDeviceSurfacePresentModesKHR(it->second, Surface(), &count, present_modes.data());

					if (_queue_family_indices.is_complete() && !surface_formats.empty() && !present_modes.empty())
					{
						physical_device = it->second;
						break;
					}
				}
			}

			if (physical_device == nullptr)
			{
				_queue_family_indices.clear();
			}
			else
			{
				ChooseSurfaceFormat(surface_formats);
				ChoosePresentMode(present_modes);
			}

			return physical_device;
		}

		VkDevice CreateLogicalDevice()
		{
			VkDevice logical_device = nullptr;

			if (_physical_device != nullptr && (VkSurfaceKHR)Surface() != nullptr)
			{
				VkPhysicalDeviceFeatures physical_features{};
				vkGetPhysicalDeviceFeatures(_physical_device, &physical_features);

				container::vector<str> enabled_extensions = GetRequiredDeviceExtensionNames();
				container::vector<const chr*> enabled_extension_names;
				for (const str& extension : enabled_extensions)
					enabled_extension_names.emplace_back(extension.c_str());

				container::vector<str> enabled_layers = GetRequiredDeviceLayerNames();
				container::vector<const chr*> enabled_layer_names;
				for (const str& layer : enabled_layers)
					enabled_layer_names.emplace_back(layer.c_str());

				container::vector<VkDeviceQueueCreateInfo> queue_infos = CreateQueueCreateInfos();

				container::vector<flt> queue_priorities{1.f};
				for (VkDeviceQueueCreateInfo& info : queue_infos)
				{
					info.queueCount = (ui32)queue_priorities.size();
					info.pQueuePriorities = queue_priorities.data();
				}

				VkDeviceCreateInfo device_info = CreateDeviceInfo();
				device_info.pEnabledFeatures = &physical_features;
				device_info.queueCreateInfoCount = queue_infos.size();
				device_info.pQueueCreateInfos = queue_infos.data();
				device_info.enabledExtensionCount = enabled_extension_names.size();
				device_info.ppEnabledExtensionNames = enabled_extension_names.data();
				device_info.enabledLayerCount = enabled_layer_names.size();
				device_info.ppEnabledLayerNames = enabled_layer_names.data();

				if (vkCreateDevice(_physical_device, &device_info, nullptr, &logical_device) != VK_SUCCESS)
				{
					logical_device = nullptr;
				}
			}

			return logical_device;
		}

	public:
		VulkanDevice(VulkanSurface& surface):
			_surface(surface),
			_physical_device(ChoosePhysicalDevice()),
			_device(CreateLogicalDevice())
		{}

		~VulkanDevice()
		{
			if (_device != nullptr)
				vkDestroyDevice(_device, nullptr);
		}

		str PhysicalDeviceName() const
		{
			str name = "";

			if (_physical_device != nullptr)
			{
				VkPhysicalDeviceProperties properties;
				vkGetPhysicalDeviceProperties(_physical_device, &properties);
				name = properties.deviceName;
			}

			return name;
		}

		VulkanInstance& Instance() const
		{
			return _surface.Instance();
		}

		VulkanSurface& Surface() const
		{
			return _surface;
		}

		VkPhysicalDevice PhysicalDevice() const
		{
			return _physical_device;
		}

		const VkSurfaceCapabilitiesKHR& Capabilities() const
		{
			return _surface_capabilities;
		}

		const VkSurfaceFormatKHR& SurfaceFormat() const
		{
			return _surface_format;
		}

		const VkPresentModeKHR& PresentMode() const
		{
			return _present_mode;
		}

		const QueueFamilyIndices_T& QueueFamilyIndices() const
		{
			return _queue_family_indices;
		}

		VkQueue GraphicsDeviceQueue() const
		{
			VkQueue queue = nullptr;
			if (_device != nullptr)
			{
				vkGetDeviceQueue(_device, _queue_family_indices.graphics.value(), 0, &queue);
			}
			return queue;
		}

		VkQueue PresentDeviceQueue() const
		{
			VkQueue queue = nullptr;
			if (_device != nullptr)
			{
				vkGetDeviceQueue(_device, _queue_family_indices.present.value(), 0, &queue);
			}
			return queue;
		}

		operator VkDevice() const
		{
			return _device;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_DEVICE_HPP */
