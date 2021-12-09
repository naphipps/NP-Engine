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
	private:
		struct QueueFamilyIndices
		{
			::std::optional<ui32> _graphics;
			::std::optional<ui32> _present;

			void clear()
			{
				_graphics.reset();
				_present.reset();
			}

			bl is_complete() const
			{
				return _graphics.has_value() && _present.has_value();
			}

			container::vector<ui32> to_vector() const
			{
				container::vector<ui32> v;

				if (is_complete())
				{
					v = {_graphics.value(), _present.value()};
				}

				return v;
			}
		};

		struct SwapchainDetails
		{
			VkSurfaceCapabilitiesKHR _capabilities;
			container::vector<VkSurfaceFormatKHR> _formats;
			container::vector<VkPresentModeKHR> _present_modes;

			void clear()
			{
				_capabilities = {};
				_formats.clear();
				_present_modes.clear();
			}

			bl is_complete() const
			{
				return !_formats.empty() && !_present_modes.empty();
			}
		};

		VulkanSurface& _surface;
		QueueFamilyIndices _queue_family_indices;
		SwapchainDetails _swapchain_details;
		VkPhysicalDevice _physical_device;
		VkSurfaceFormatKHR _surface_format;
		VkPresentModeKHR _present_mode;
		VkExtent2D _swap_extent;
		VkDevice _logical_device;
		VkSwapchainKHR _swapchain;
		container::vector<VkImage> _swapchain_images;
		container::vector<VkImageView> _swapchain_image_views;

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
			vkEnumeratePhysicalDevices(GetInstance().GetVkInstance(), &count, nullptr);
			container::vector<VkPhysicalDevice> devices(count);
			vkEnumeratePhysicalDevices(GetInstance().GetVkInstance(), &count, devices.data());
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
			return {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
		}

		container::vector<str> GetRequiredDeviceLayerNames() const
		{
			return {};
		}

		ui32 GetPhysicalDeviceScore(VkPhysicalDevice physical_device)
		{
			ui32 score = 0;

			if (physical_device != nullptr && GetSurface().GetVkSurface() != nullptr)
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

				bl has_queue_graphics_bit = false; // we require the VK_QUEUE_GRAPHICS_BIT
				bl has_queue_present_bit = false; // we require KHR present support
				bl supports_required_extensions = false;
				bl supports_required_layers = false;

				// check queue families
				{
					container::vector<VkQueueFamilyProperties> queue_family_properties =
						GetQueueFamilyProperties(physical_device);

					for (siz i = 0; i < queue_family_properties.size(); i++)
					{
						if (!has_queue_graphics_bit && (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
						{
							has_queue_graphics_bit = true;
						}

						if (!has_queue_present_bit)
						{
							VkBool32 supported = false;
							vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, GetSurface().GetVkSurface(), &supported);
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

				if (!has_queue_graphics_bit || !has_queue_present_bit || !supports_required_extensions ||
					!supports_required_layers)
				{
					score = 0;
				}
			}

			return score;
		}

		VkSwapchainCreateInfoKHR CreateSwapchainCreateInfo()
		{
			VkSwapchainCreateInfoKHR info{};
			info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			info.surface = GetSurface().GetVkSurface();
			info.imageFormat = _surface_format.format;
			info.imageColorSpace = _surface_format.colorSpace;
			info.imageExtent = _swap_extent;
			info.imageArrayLayers = 1;
			info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			return info;
		}

		container::vector<VkDeviceQueueCreateInfo> CreateQueueCreateInfos()
		{
			container::vector<VkDeviceQueueCreateInfo> infos;

			if (_physical_device != nullptr && _queue_family_indices.is_complete())
			{
				container::oset<ui32> families;
				families.emplace(_queue_family_indices._graphics.value());
				families.emplace(_queue_family_indices._present.value());

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

		VkDeviceCreateInfo CreateDeviceCreateInfo()
		{
			VkDeviceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			return info;
		}

		VkImageViewCreateInfo CreateImageViewCreateInfo()
		{
			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = _surface_format.format;
			info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.baseMipLevel = 0;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.baseArrayLayer = 0;
			info.subresourceRange.layerCount = 1;
			return info;
		}

		VkPhysicalDevice ChoosePhysicalDevice()
		{
			VkPhysicalDevice physical_device = nullptr;
			container::ommap<i32, VkPhysicalDevice> candidates;
			for (VkPhysicalDevice device : GetPhysicalDevices())
				candidates.emplace(GetPhysicalDeviceScore(device), device);

			if (GetSurface().GetVkSurface() != nullptr && candidates.size() > 0)
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
						if (!_queue_family_indices._graphics.has_value() &&
							queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
						{
							_queue_family_indices._graphics = (ui32)i;
						}

						if (!_queue_family_indices._present.has_value())
						{
							VkBool32 supported = VK_FALSE;
							vkGetPhysicalDeviceSurfaceSupportKHR(it->second, i, GetSurface().GetVkSurface(), &supported);
							if (supported == VK_TRUE)
								_queue_family_indices._present = (ui32)i;
						}
					}

					vkGetPhysicalDeviceSurfaceCapabilitiesKHR(it->second, GetSurface().GetVkSurface(),
															  &_swapchain_details._capabilities);

					ui32 count;
					vkGetPhysicalDeviceSurfaceFormatsKHR(it->second, GetSurface().GetVkSurface(), &count, nullptr);
					_swapchain_details._formats.resize(count);
					vkGetPhysicalDeviceSurfaceFormatsKHR(it->second, GetSurface().GetVkSurface(), &count,
														 _swapchain_details._formats.data());

					vkGetPhysicalDeviceSurfacePresentModesKHR(it->second, GetSurface().GetVkSurface(), &count, nullptr);
					_swapchain_details._present_modes.resize(count);
					vkGetPhysicalDeviceSurfacePresentModesKHR(it->second, GetSurface().GetVkSurface(), &count,
															  _swapchain_details._present_modes.data());

					if (_queue_family_indices.is_complete() && _swapchain_details.is_complete())
					{
						physical_device = it->second;
						break;
					}
				}
			}

			if (physical_device == nullptr)
			{
				_queue_family_indices.clear();
				_swapchain_details.clear();
			}

			return physical_device;
		}

		VkSurfaceFormatKHR ChooseSurfaceFormat()
		{
			VkSurfaceFormatKHR format{};

			if (_physical_device != nullptr)
			{
				format = _swapchain_details._formats[0];

				// TODO: may need to rank formats to choose best one
				for (VkSurfaceFormatKHR& f : _swapchain_details._formats)
				{
					if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
					{
						format = f;
						break;
					}
				}
			}

			return format;
		}

		VkPresentModeKHR ChoosePresentMode()
		{
			VkPresentModeKHR mode{};

			if (_physical_device != nullptr)
			{
				mode = VK_PRESENT_MODE_FIFO_KHR;

				// TODO: if we ever decide to support mobile, then we'll want to use fifo
				for (VkPresentModeKHR& p : _swapchain_details._present_modes)
				{
					if (p == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						mode = p;
						break;
					}
				}
			}

			return mode;
		}

		VkExtent2D ChooseSwapExtent()
		{
			VkExtent2D extent{};

			if (_physical_device != nullptr)
			{
				if (_swapchain_details._capabilities.currentExtent.width != UI32_MAX)
				{
					extent = _swapchain_details._capabilities.currentExtent;
				}
				else
				{
					i32 width, height;
					glfwGetFramebufferSize((GLFWwindow*)GetSurface().GetWindow().GetNativeWindow(), &width, &height);

					VkExtent2D min_extent = _swapchain_details._capabilities.minImageExtent;
					VkExtent2D max_extent = _swapchain_details._capabilities.maxImageExtent;

					extent = {::std::clamp((ui32)width, min_extent.width, max_extent.width),
							  ::std::clamp((ui32)height, min_extent.height, max_extent.height)};
				};
			}

			return extent;
		}

		VkDevice CreateLogicalDevice()
		{
			VkDevice logical_device = nullptr;

			if (_physical_device != nullptr && GetSurface().GetVkSurface() != nullptr)
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

				container::vector<VkDeviceQueueCreateInfo> queue_create_infos = CreateQueueCreateInfos();

				container::vector<flt> queue_priorities{1.f};
				for (VkDeviceQueueCreateInfo& info : queue_create_infos)
				{
					info.queueCount = (ui32)queue_priorities.size();
					info.pQueuePriorities = queue_priorities.data();
				}

				VkDeviceCreateInfo device_create_info = CreateDeviceCreateInfo();
				device_create_info.pEnabledFeatures = &physical_features;
				device_create_info.queueCreateInfoCount = queue_create_infos.size();
				device_create_info.pQueueCreateInfos = queue_create_infos.data();
				device_create_info.enabledExtensionCount = enabled_extension_names.size();
				device_create_info.ppEnabledExtensionNames = enabled_extension_names.data();
				device_create_info.enabledLayerCount = enabled_layer_names.size();
				device_create_info.ppEnabledLayerNames = enabled_layer_names.data();

				if (vkCreateDevice(_physical_device, &device_create_info, nullptr, &logical_device) != VK_SUCCESS)
				{
					logical_device = nullptr;
				}
			}

			return logical_device;
		}

		VkSwapchainKHR CreateSwapchain()
		{
			VkSwapchainKHR swapchain = nullptr;

			if (_logical_device != nullptr)
			{
				ui32 min_image_count = _swapchain_details._capabilities.minImageCount + 1;
				if (_swapchain_details._capabilities.maxImageCount != 0)
					min_image_count = ::std::min(min_image_count, _swapchain_details._capabilities.maxImageCount);

				container::vector<ui32> indices = _queue_family_indices.to_vector();

				VkSwapchainCreateInfoKHR swapchain_create_info = CreateSwapchainCreateInfo();
				swapchain_create_info.minImageCount = min_image_count;

				if (_queue_family_indices._graphics != _queue_family_indices._present)
				{
					swapchain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
					swapchain_create_info.queueFamilyIndexCount = (ui32)indices.size();
					swapchain_create_info.pQueueFamilyIndices = indices.data();
				}
				else
				{
					swapchain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				}

				// says that we don't want any local transform
				swapchain_create_info.preTransform = _swapchain_details._capabilities.currentTransform;
				swapchain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
				swapchain_create_info.presentMode = _present_mode;
				swapchain_create_info.clipped = VK_TRUE;
				swapchain_create_info.oldSwapchain = nullptr;

				if (vkCreateSwapchainKHR(_logical_device, &swapchain_create_info, nullptr, &swapchain) != VK_SUCCESS)
				{
					swapchain = nullptr;
				}
			}

			return swapchain;
		}

		container::vector<VkImage> RetrieveSwapchainImages()
		{
			container::vector<VkImage> images;

			if (_swapchain != nullptr)
			{
				ui32 count;
				vkGetSwapchainImagesKHR(_logical_device, _swapchain, &count, nullptr);
				images.resize(count);
				vkGetSwapchainImagesKHR(_logical_device, _swapchain, &count, images.data());
			}

			return images;
		}

		container::vector<VkImageView> CreateSwapchainImageViews()
		{
			container::vector<VkImageView> views(_swapchain_images.size());

			for (siz i = 0; i < _swapchain_images.size(); i++)
			{
				VkImageViewCreateInfo image_view_create_info = CreateImageViewCreateInfo();
				image_view_create_info.image = _swapchain_images[i];
				if (vkCreateImageView(_logical_device, &image_view_create_info, nullptr, &views[i]) != VK_SUCCESS)
				{
					views.clear();
					break;
				}
			}

			return views;
		}

	public:
		VulkanDevice(VulkanSurface& surface):
			_surface(surface),
			_physical_device(ChoosePhysicalDevice()),
			_surface_format(ChooseSurfaceFormat()),
			_present_mode(ChoosePresentMode()),
			_swap_extent(ChooseSwapExtent()),
			_logical_device(CreateLogicalDevice()),
			_swapchain(CreateSwapchain()),
			_swapchain_images(RetrieveSwapchainImages()),
			_swapchain_image_views(CreateSwapchainImageViews())
		{}

		~VulkanDevice()
		{
			for (VkImageView& view : _swapchain_image_views)
				vkDestroyImageView(_logical_device, view, nullptr);

			if (_swapchain != nullptr)
				vkDestroySwapchainKHR(_logical_device, _swapchain, nullptr);

			if (_logical_device != nullptr)
				vkDestroyDevice(_logical_device, nullptr);
		}

		str GetPhysicalDeviceName() const
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

		VulkanInstance& GetInstance()
		{
			return _surface.GetInstance();
		}

		VulkanInstance& GetInstance() const
		{
			return _surface.GetInstance();
		}

		VulkanSurface& GetSurface()
		{
			return _surface;
		}

		VulkanSurface& GetSurface() const
		{
			return _surface;
		}

		VkPhysicalDevice GetPhysicalDevice() const
		{
			return _physical_device;
		}

		VkSwapchainKHR GetVkSwapchain()
		{
			return _swapchain;
		}

		VkSwapchainKHR GetVkSwapchain() const
		{
			return _swapchain;
		}

		container::vector<VkImage>& GetSwapchainImages()
		{
			return _swapchain_images;
		}

		const container::vector<VkImage>& GetSwapchainImages() const
		{
			return _swapchain_images;
		}

		container::vector<VkImageView>& GetSwapchainImageViews()
		{
			return _swapchain_image_views;
		}

		const container::vector<VkImageView>& GetSwapchainImageViews() const
		{
			return _swapchain_image_views;
		}

		VkDevice GetLogicalDevice() const
		{
			return _logical_device;
		}

		VkQueue GetGraphicsDeviceQueue() const
		{
			VkQueue queue = nullptr;
			if (IsEnabled())
			{
				vkGetDeviceQueue(GetLogicalDevice(), _queue_family_indices._graphics.value(), 0, &queue);
			}
			return queue;
		}

		VkQueue GetPresentDeviceQueue() const
		{
			VkQueue queue = nullptr;
			if (IsEnabled())
			{
				vkGetDeviceQueue(GetLogicalDevice(), _queue_family_indices._present.value(), 0, &queue);
			}
			return queue;
		}

		bl IsEnabled() const
		{
			return _physical_device != nullptr && _queue_family_indices.is_complete() && _logical_device != nullptr;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_DEVICE_HPP */
