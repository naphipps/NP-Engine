//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/5/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_DEVICE_HPP
#define NP_ENGINE_VULKAN_RENDER_DEVICE_HPP

#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanRenderTarget.hpp"
#include "VulkanLogicalDevice.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanQueue.hpp"

namespace np::gpu::__detail
{
	class VulkanRenderDevice : public RenderDevice
	{
	private:
		VkPhysicalDevice _physical_device;
		ui32 _graphics_family_index;
		ui32 _present_family_index;
		VkSurfaceFormatKHR _surface_format;
		VkPresentModeKHR _present_mode;
		mem::sptr<VulkanLogicalDevice> _device;
		mem::sptr<VulkanCommandPool> _command_pool;
		mem::sptr<VulkanQueue> _graphics_queue;
		mem::sptr<VulkanQueue> _present_queue;

		static siz GetPhysicalDeviceScore(VkPhysicalDevice physical_device, mem::sptr<RenderTarget> target)
		{
			siz score = 0;

			if (physical_device && target)
			{
				VulkanRenderTarget& render_target = (VulkanRenderTarget&)(*target);

				VkPhysicalDeviceProperties properties{};
				VkPhysicalDeviceVulkan12Properties properties12{};
				VkPhysicalDeviceProperties2 properties2{};
				properties12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
				properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
				properties2.pNext = &properties12;
				vkGetPhysicalDeviceProperties(physical_device, &properties);
				vkGetPhysicalDeviceProperties2(physical_device, &properties2);

				VkPhysicalDeviceFeatures features{};
				VkPhysicalDeviceVulkan12Features features12{};
				VkPhysicalDeviceFeatures2 features2{};
				features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
				features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
				features2.pNext = &features12;
				vkGetPhysicalDeviceFeatures(physical_device, &features);
				vkGetPhysicalDeviceFeatures2(physical_device, &features2);

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

				if (features.samplerAnisotropy == VK_TRUE)
					score += 200;

				bl has_queue_graphics_bit = false; // we require the VK_QUEUE_GRAPHICS_BIT
				bl has_queue_present_bit = false; // we require KHR present support
				bl supports_required_extensions = false;
				bl supports_required_layers = false;

				// check queue families
				{
					con::vector<VkQueueFamilyProperties> queue_family_properties = VulkanLogicalDevice::GetQueueFamilyProperties(physical_device);

					for (siz i = 0; i < queue_family_properties.size(); i++)
					{
						if (!has_queue_graphics_bit && (queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
						{
							has_queue_graphics_bit = true;
						}

						if (!has_queue_present_bit)
						{
							VkBool32 supported = false;
							vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, render_target, &supported);
							has_queue_present_bit = (bl)supported;
						}
					}
				}

				// check extensions
				{
					con::vector<str> required_extensions = VulkanLogicalDevice::GetRequiredDeviceExtensionNames();
					if (required_extensions.size() > 0)
					{
						con::uset<str> required_extensions_set(required_extensions.begin(), required_extensions.end());
						con::vector<str> supported_extensions = VulkanLogicalDevice::GetSupportedDeviceExtensionNames(physical_device);
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
					con::vector<str> required_layers = VulkanLogicalDevice::GetRequiredDeviceLayerNames();
					if (required_layers.size() > 0)
					{
						con::uset<str> required_layers_set(required_layers.begin(), required_layers.end());
						con::vector<str> supported_layers = VulkanLogicalDevice::GetSupportedDeviceLayerNames(physical_device);
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

		static VkPhysicalDevice ChoosePhysicalDevice(mem::sptr<RenderTarget> render_target)
		{
			VkPhysicalDevice physical_device = nullptr;

			if (render_target)
			{
				mem::sptr<DetailInstance> instance = render_target->GetInstance();

				using CandidateType = ::std::pair<siz, VkPhysicalDevice>;
				con::vector<VkPhysicalDevice> physical_devices = VulkanLogicalDevice::GetPhysicalDevices(instance);
				con::vector<CandidateType> candidates;

				for (VkPhysicalDevice pd : physical_devices)
					candidates.emplace_back(GetPhysicalDeviceScore(pd, render_target), pd);

				::std::sort(candidates.begin(), candidates.end(),
					[](const CandidateType& a, const CandidateType& b) {
						return a.first > b.first; //descending sort
					}
				);

				if (!candidates.empty() && candidates.front().first > 0)
					physical_device = candidates.front().second;
			}

			return physical_device;
		}

		static ui32 GetGraphicsFamilyIndex(VkPhysicalDevice physical_device)
		{
			::std::optional<ui32> graphics_family_index;
			graphics_family_index.reset(); //ensure it is reset

			ui32 count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);
			con::vector<VkQueueFamilyProperties> queue_families(count);
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, queue_families.data());

			for (siz i = 0; i < queue_families.size() && !graphics_family_index.has_value(); i++)
				if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) //TODO: we may want to compare other queue families?
					graphics_family_index = (ui32)i;

			NP_ENGINE_ASSERT(graphics_family_index.has_value(), "we require graphics family index");
			return graphics_family_index.value();
		}

		static ui32 GetPresentFamilyIndex(VkPhysicalDevice physical_device, mem::sptr<RenderTarget> target)
		{
			VulkanRenderTarget& render_target = (VulkanRenderTarget&)(*target);
			::std::optional<ui32> present_family_index;
			present_family_index.reset();

			ui32 count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, nullptr);
			con::vector<VkQueueFamilyProperties> queue_families(count);
			vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &count, queue_families.data());

			for (siz i = 0; i < queue_families.size() && !present_family_index.has_value(); i++)
			{
				VkBool32 supported = VK_FALSE;
				vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, render_target, &supported);
				if (supported == VK_TRUE)
					present_family_index = (ui32)i;
			}

			NP_ENGINE_ASSERT(present_family_index.has_value(), "we require present family index");
			return present_family_index.value();
		}

		static con::vector<VkDeviceQueueCreateInfo> CreateQueueCreateInfos(VkPhysicalDevice physical_device, con::oset<ui32> families)
		{
			con::vector<VkDeviceQueueCreateInfo> infos;

			if (physical_device)
			{
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

		static VkSurfaceFormatKHR ChooseSurfaceFormat(VkPhysicalDevice physical_device, mem::sptr<RenderTarget> target)
		{
			VkSurfaceFormatKHR surface_format = {};

			if (physical_device)
			{
				VulkanRenderTarget& render_target = (VulkanRenderTarget&)(*target);

				ui32 count;
				vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, render_target, &count, nullptr);
				con::vector<VkSurfaceFormatKHR> surface_formats(count);
				vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, render_target, &count, surface_formats.data());

				surface_format = surface_formats.front(); //default

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

		static VkPresentModeKHR ChoosePresentMode(VkPhysicalDevice physical_device, mem::sptr<RenderTarget> target)
		{
			VkPresentModeKHR present_mode = {};

			if (physical_device)
			{
				VulkanRenderTarget& render_target = (VulkanRenderTarget&)(*target);

				ui32 count;
				vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, render_target, &count, nullptr);
				con::vector<VkPresentModeKHR> present_modes(count);
				vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, render_target, &count, present_modes.data());

				present_mode = VK_PRESENT_MODE_FIFO_KHR; //default

				// TODO: if we ever decide to support mobile, then we'll want to use fifo
				for (VkPresentModeKHR& p : present_modes)
					if (p == VK_PRESENT_MODE_MAILBOX_KHR)
					{
						present_mode = p;
						break;
					}
			}

			return present_mode;
		}

		static mem::sptr<VulkanLogicalDevice> CreateLogicalDevice(
			mem::sptr<srvc::Services> services, 
			VkPhysicalDevice physical_device, 
			con::vector<VkDeviceQueueCreateInfo> queue_infos)
		{
			return mem::create_sptr<VulkanLogicalDevice>(services->GetAllocator(), services, physical_device, queue_infos);
		}

		static VkCommandPoolCreateInfo CreateCommandPoolInfo(ui32 queue_family_index)
		{
			VkCommandPoolCreateInfo info = VulkanCommandPool::CreateInfo();
			info.queueFamilyIndex = queue_family_index;
			info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			return info;
		}

		static mem::sptr<VulkanCommandPool> CreateCommandPool(
			mem::sptr<srvc::Services> services, 
			mem::sptr<VulkanLogicalDevice> device, 
			VkCommandPoolCreateInfo info)
		{
			return mem::create_sptr<VulkanCommandPool>(services->GetAllocator(), device, info);
		}

		static mem::sptr<VulkanQueue> CreateQueue(
			mem::sptr<srvc::Services> services,
			mem::sptr<VulkanLogicalDevice> device, 
			ui32 queue_family_index, 
			ui32 queue_index)
		{
			return mem::create_sptr<VulkanQueue>(services->GetAllocator(), device, queue_family_index, queue_index);
		}

	public:
		VulkanRenderDevice(mem::sptr<RenderTarget> target) :
			RenderDevice(target),
			_physical_device(ChoosePhysicalDevice(target)),
			_graphics_family_index(GetGraphicsFamilyIndex(_physical_device)),
			_present_family_index(GetPresentFamilyIndex(_physical_device, target)),
			_surface_format(ChooseSurfaceFormat(_physical_device, target)),
			_present_mode(ChoosePresentMode(_physical_device, target)),
			_device(CreateLogicalDevice(GetServices(), _physical_device,
				CreateQueueCreateInfos(_physical_device, { _graphics_family_index, _present_family_index }))),
			_command_pool(CreateCommandPool(GetServices(), _device, CreateCommandPoolInfo(_graphics_family_index))),
			_graphics_queue(CreateQueue(GetServices(), _device, _graphics_family_index, 0)),
			_present_queue(CreateQueue(GetServices(), _device, _present_family_index, 0))
		{
			NP_ENGINE_ASSERT(GetDetailType() == DetailType::Vulkan,
				"VulkanRenderDevice required given RenderTarget to be gpu::DetailType::Vulkan");
		}

		operator VkDevice() const
		{
			return *_device;
		}

		VkSurfaceFormatKHR GetSurfaceFormat() const
		{
			return _surface_format;
		}

		VkPresentModeKHR GetPresentMode() const
		{
			return _present_mode;
		}

		ui32 GetGraphicsFamilyIndex() const
		{
			return _graphics_family_index;
		}

		ui32 GetPresentFamilyIndex() const
		{
			return _present_family_index;
		}

		mem::sptr<VulkanQueue> GetGraphicsQueue() const
		{
			return _graphics_queue;
		}

		mem::sptr<VulkanQueue> GetPresentQueue() const
		{
			return _present_queue;
		}

		str GetPhysicalDeviceName() const
		{
			return _device->GetPhysicalDeviceName();
		}

		VkPhysicalDevice GetPhysicalDevice() const
		{
			return _physical_device;
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _device;
		}

		mem::sptr<VulkanCommandPool> GetCommandPool() const
		{
			return _command_pool;
		}
	};
}

#endif /* NP_ENGINE_VULKAN_RENDER_DEVICE_HPP */