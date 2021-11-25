//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/21/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_DEVICE_HPP
#define NP_ENGINE_VULKAN_DEVICE_HPP

#include <iostream> //TODO: remove

#include <vulkan/vulkan.hpp> //TODO: figure out when we want to include our vendor headers...

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Window/Window.hpp"

namespace np::graphics::rhi
{
	class VulkanDevice
	{
	private:

		constexpr static ui32 REQUIRED_VERSION = VK_MAKE_API_VERSION(0, 1, 2, 162);

		memory::Allocator& _allocator;
		window::Window& _window;

		VkInstance _instance;
		VkPhysicalDevice _physical_device;
		ui32 _queue_family_index;
		VkDevice _logical_device;

		void* _surface;


		str _vk_VersionToStr(ui32 version) const
		{
			return to_str(VK_API_VERSION_MAJOR(version)) + "." +
				to_str(VK_API_VERSION_MINOR(version)) + "." +
				to_str(VK_API_VERSION_PATCH(version)) + "." +
				to_str(VK_API_VERSION_VARIANT(version));
		}

		ui32 _vk_GetVersion() const
		{
			//return VK_HEADER_VERSION_COMPLETE; //1.2.189.0
			ui32 version = 0;
			vkEnumerateInstanceVersion(&version); //version: 4202658, 1.2.162.0
			return version;
		}

		container::vector<VkExtensionProperties> _vk_GetSupportedExtensions() const
		{
			ui32 count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
			container::vector<VkExtensionProperties> extensions(count);
			vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
			return extensions;
		}

		container::vector<str> _vk_GetSupportedExtensionNames() const
		{
			container::vector<VkExtensionProperties> extensions = _vk_GetSupportedExtensions();
			container::vector<str> names;
			for (auto e : extensions) names.emplace_back(e.extensionName);
			return names;
		}

		container::vector<VkLayerProperties> _vk_GetSupportedLayers() const
		{
			ui32 count;
			vkEnumerateInstanceLayerProperties(&count, nullptr);
			container::vector<VkLayerProperties> layers(count);
			vkEnumerateInstanceLayerProperties(&count, layers.data());
			return layers;
		}

		container::vector<str> _vk_GetSupportedLayerNames() const
		{
			container::vector<VkLayerProperties> layers = _vk_GetSupportedLayers();
			container::vector<str> names;
			for (auto e : layers) names.emplace_back(e.layerName);
			return names;
		}

		container::vector<VkPhysicalDevice> _vk_GetPhysicalDevices() const
		{
			ui32 count = 0;
			vkEnumeratePhysicalDevices(_instance, &count, nullptr);
			container::vector<VkPhysicalDevice> devices(count);
			vkEnumeratePhysicalDevices(_instance, &count, devices.data());
			return devices;
		}

		container::vector<VkQueueFamilyProperties> _vk_GetQueueFamilyProperties(VkPhysicalDevice device) const
		{
			ui32 count = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);
			container::vector<VkQueueFamilyProperties> properties(count);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &count, properties.data());
			return properties;
		}

		container::vector<str> GetRequiredExtensionNames() const
		{
			container::vector<str> extensions;

			ui32 count;
			const chr** glfw_required = glfwGetRequiredInstanceExtensions(&count);

			for (ui32 i = 0; i < count; i++)
			{
				extensions.emplace_back(glfw_required[i]);
			}

			//TODO: any more extensions we require? we could just require all of the extensions?

			return extensions;
		}

		container::vector<str> GetRequiredLayerNames() const
		{
			container::vector<str> layers;

			return layers;
		}

		VkInstance CreateInstance()
		{
			VkInstance instance = nullptr;

			VkApplicationInfo application_info;
			application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			application_info.pApplicationName = "NP Engine Application - Vulkan Instance";
			application_info.applicationVersion = VK_MAKE_VERSION(1, 2, 3); //TODO: get version from cmake
			application_info.pEngineName = "NP Engine";
			application_info.engineVersion = VK_MAKE_VERSION(2, 3, 4); //TODO: get version from cmake
			application_info.apiVersion = REQUIRED_VERSION;

			container::vector<str> required_extensions = GetRequiredExtensionNames();
			memory::Block block = _allocator.Allocate(sizeof(chr*) * required_extensions.size());
			memory::ConstructArray<chr*>(block, required_extensions.size());
			chr** required_names = (chr**)block.ptr;

			for (siz i = 0; i < required_extensions.size(); i++)
			{
				block = _allocator.Allocate(sizeof(chr) * required_extensions[i].size());
				memory::ConstructArray<chr>(block, required_extensions[i].size());
				required_names[i] = (chr*)block.ptr;
				::std::strcpy(required_names[i], required_extensions[i].c_str());
			}

			VkInstanceCreateInfo create_info;
			create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			create_info.pApplicationInfo = &application_info;
			create_info.enabledExtensionCount = required_extensions.size();
			create_info.ppEnabledExtensionNames = required_names;
			create_info.enabledLayerCount = 0;
			create_info.ppEnabledLayerNames = nullptr;
			create_info.flags = 0; //per spec, is not used and is suppose to be set to zero
			create_info.pNext = nullptr; //for some reason this does not default init to nullptr, so we set

			if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS)
			{
				::std::cout << "create instance failed\n";
				instance = nullptr;
			}

			for (siz i = 0; i < required_extensions.size(); i++)
			{
				memory::DestructArray(required_names[i], required_extensions[i].size());
				_allocator.Deallocate(required_names[i]);
			}

			memory::DestructArray(required_names, required_extensions.size());
			_allocator.Deallocate(required_names);

			return instance;
		}

		ui32 GetPhysicalDeviceScore(VkPhysicalDevice physical_device)
		{
			ui32 score = 0;

			if (physical_device)
			{
				VkPhysicalDeviceProperties properties;
				VkPhysicalDeviceFeatures features;
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
				}


				//TODO: go through all properties and features to determine best score...

				//TODO: dealbreakers listed here...


				//we require the VK_QUEUE_GRAPHICS_BIT
				bl has_queue_graphics_bit = false;
				_queue_family_index = -1;
				ui32 index = 0;
				container::vector<VkQueueFamilyProperties> queue_family_properties = _vk_GetQueueFamilyProperties(physical_device);
				for (const VkQueueFamilyProperties& queue_family : queue_family_properties)
				{
					if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
					{
						has_queue_graphics_bit = true;
						_queue_family_index = index;
						break;
					}

					index++;
				}

				if (!has_queue_graphics_bit)
				{
					score = 0;
				}
			}

			return score;
		}

		void* CreateSurface()
		{
			return nullptr;
		}

		VkPhysicalDevice ChoosePhysicalDevice()
		{
			//TODO: maybe we can use a multimap to store all scores, then decide at the very end, instead of choosing as we loop

			VkPhysicalDevice physical_device = nullptr;
			ui32 score = 0;

			if (_instance != nullptr)
			{
				for (VkPhysicalDevice device : _vk_GetPhysicalDevices())
				{
					ui32 device_score = GetPhysicalDeviceScore(device);
					if (score < device_score)
					{
						physical_device = device;
						score = device_score;
					}
				}
			}

			return score > 0 ? physical_device : nullptr;
		}

		VkDevice CreateLogicalDevice()
		{
			VkDevice logical_device = nullptr;

			if (_physical_device)
			{
				VkPhysicalDeviceFeatures physical_features;
				vkGetPhysicalDeviceFeatures(_physical_device, &physical_features);

				//VkQueueFamilyProperties queue_family_properties = _vk_GetQueueFamilyProperties(_physical_device)[_queue_family_index];
				flt queue_priorities[1] = { 1.f };

				VkDeviceQueueCreateInfo queue_create_info;
				queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queue_create_info.flags = 0; //TODO: assuming same as physical device
				queue_create_info.pNext = nullptr;
				queue_create_info.queueFamilyIndex = _queue_family_index;
				queue_create_info.queueCount = 1;
				queue_create_info.pQueuePriorities = queue_priorities;
				
				VkDeviceCreateInfo device_create_info;
				device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
				device_create_info.flags = 0;
				device_create_info.pNext = nullptr;
				device_create_info.queueCreateInfoCount = 1;
				device_create_info.pQueueCreateInfos = &queue_create_info;
				device_create_info.pEnabledFeatures = &physical_features;
				
				//fields here for legacy reasons, we will not support legacy implementations, so we set 0 and nullptr
				device_create_info.enabledExtensionCount = 0;
				device_create_info.ppEnabledExtensionNames = nullptr;
				device_create_info.enabledLayerCount = 0;
				device_create_info.ppEnabledLayerNames = nullptr;


				if (vkCreateDevice(_physical_device, &device_create_info, nullptr, &logical_device) != VK_SUCCESS)
				{
					logical_device = nullptr;
				}
			}

			return logical_device;
		}

	public:

		VulkanDevice(window::Window& window):
		_allocator(memory::DefaultTraitAllocator),
		_window(window),
		_surface(CreateSurface()),
		_instance(CreateInstance()),
		_physical_device(ChoosePhysicalDevice()),
		_logical_device(CreateLogicalDevice())
		{}

		~VulkanDevice()
		{
			if (_surface != nullptr)
			{
				//TODO: destroy surface
			}

			if (_logical_device != nullptr)
			{
				vkDestroyDevice(_logical_device, nullptr);
			}

			if (_instance != nullptr)
			{
				vkDestroyInstance(_instance, nullptr);
			}
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

		VkDevice GetLogicalDevice() const
		{
			return _logical_device;
		}

		VkQueue GetLogicalDeviceQueue() const
		{
			VkQueue queue = nullptr;
			if (IsEnabled())
			{
				vkGetDeviceQueue(GetLogicalDevice(), _queue_family_index, 0, &queue);
			}
			return queue;
		}

		bl IsEnabled() const
		{
			return _surface != nullptr && _instance != nullptr && _physical_device != nullptr && _logical_device != nullptr;
		}
	};
}

#endif /* NP_ENGINE_VULKAN_DEVICE_HPP */