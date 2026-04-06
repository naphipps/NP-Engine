//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/5/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_INSTANCE_HPP
#define NP_ENGINE_GPU_VULKAN_INSTANCE_HPP

#include <iostream>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Window/Window.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Detail.hpp"

#include "VulkanAllocationCallbacks.hpp"

/*
	TODO: create VulkanAllocator. Below are some thoughts:
		- since vulkan requires specific alignment, alloc could look like this:
			void* alloc(userData, size, alignment, scope)
			{
				allocate block with sub-blocks like:
				[[magic number byte][zeroized bytes][vulkan block for requested size starting on multiple of requested
   alignment]] size of magic-number-block.size shall be 1 size of magic-number-block.size + zeroized-block.size shall be
   multiple of our ALIGNMENT

				RETURN pointer where vulkan block starts

				^This way, vulkan block can start on pointer that is a multiple of requested alignment
					AND we can just navigate before that to find our magic number. The address of that magic-number-byte
					shall be the address our allocators produce. So we need to calculate an aligned size for
					all these sub-blocks, allocate that from our allocators, mark the magic number, zeroize
					the remaining bytes up to where the vulkan block starts, then return where that vulkan block starts.
					THEN reverse for free, account for realloc, etc.

				Magic number can be 15, 1, ~0, or something. Does not matter. Just NOT zero.
			}
		- take advantage of those alloc/free notification callbacks
*/

namespace np::gpu::__detail
{
	class VulkanInstance : public DetailInstance
	{
	private:
		constexpr static ui32 REQUIRED_VERSION = VK_MAKE_API_VERSION(0, 1, 2, 189);

		mem::sptr<srvc::Services> _services;
		VulkanAllocationCallbacks _allocation_callbacks;
		VkInstance _instance;
		VkDebugUtilsMessengerEXT _debug_messenger;

		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
																  VkDebugUtilsMessageTypeFlagsEXT msg_type,
																  const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
																  void* user_data)
		{
			// TODO: feel like we should pipe this stuff through our logger
			// TODO: should we have a macro to enable this? NP_ENGINE_ENABLE_VULKAN_DEBUG_CALLBACK?
			// TODO: do we still have these known messages?

			con::vector<str> known_msgs{
				R"(loader_scanned_icd_add: Driver C:\Windows\System32\DriverStore\FileRepository\u0377495.inf_amd64_58cc395c0bf03a26\B377432\.\amdvlk64.dll says it supports interface version 6 but still exports core entrypoints (Policy #LDP_DRIVER_6))" // vulkansdk 1.3.211
				,
				"Layer VK_LAYER_RTSS uses API version 1.1 which is older than the application specified API version of 1.2. "
				"May cause issues."};

			VkBool32 retval = VK_TRUE;
			bl found = false;

			for (auto it = known_msgs.begin(); !found && it != known_msgs.end(); it++)
				found = *it == callback_data->pMessage;

			if (!found)
			{
				if ((msg_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) != 0 ||
					(msg_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) != 0)
				{
					retval = VK_FALSE;
					str msg = "NP Validation Layer: ";
					msg += "[[" + to_str(callback_data->pMessageIdName) + ", " + to_str(callback_data->messageIdNumber) + "]] ";
					msg += to_str(callback_data->pMessage);

					NP_ENGINE_LOG_CRITICAL(msg);
					// NP_ENGINE_ASSERT(false, msg); // here in case for issues that cause a gpu crash
				}
			}

			return retval;
		}

		str Version32ToStr(ui32 version) const
		{
			return to_str(VK_API_VERSION_MAJOR(version)) + "." + to_str(VK_API_VERSION_MINOR(version)) + "." +
				to_str(VK_API_VERSION_PATCH(version)) + "." + to_str(VK_API_VERSION_VARIANT(version));
		}

		ui32 GetVersion32() const
		{
			// TODO: investigate what we should return, etc
			//  return VK_HEADER_VERSION_COMPLETE; //1.2.189.0
			ui32 version = 0;
			vkEnumerateInstanceVersion(&version); // version: 4202658, 1.2.162.0
			return version;
		}

		con::vector<VkExtensionProperties> GetVkSupportedInstanceExtensions() const
		{
			ui32 count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
			con::vector<VkExtensionProperties> extensions(count);
			vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
			return extensions;
		}

		con::vector<str> GetSupportedInstanceExtensionNames() const
		{
			con::vector<VkExtensionProperties> extensions = GetVkSupportedInstanceExtensions();
			con::vector<str> names;
			for (const VkExtensionProperties& e : extensions)
				names.emplace_back(e.extensionName);
			return names;
		}

		con::vector<VkLayerProperties> GetVkSupportedInstanceLayers() const
		{
			ui32 count;
			vkEnumerateInstanceLayerProperties(&count, nullptr);
			con::vector<VkLayerProperties> layers(count);
			vkEnumerateInstanceLayerProperties(&count, layers.data());
			return layers;
		}

		con::vector<str> GetSupportedInstanceLayerNames() const
		{
			con::vector<VkLayerProperties> layers = GetVkSupportedInstanceLayers();
			con::vector<str> names;
			for (const VkLayerProperties& e : layers)
				names.emplace_back(e.layerName);
			return names;
		}

		con::vector<str> GetRequiredInstanceExtensionNames() const
		{
			con::uset<str> extension_set;
			extension_set.emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

#if NP_ENGINE_PLATFORM_IS_APPLE
			extension_set.emplace(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
#endif

			con::vector<str> window_required = win::Window::GetRequiredGpuExtentions(win::DetailType::Glfw);
			for (const str& extension : window_required)
				extension_set.emplace(extension);

			window_required = win::Window::GetRequiredGpuExtentions(win::DetailType::Sdl);
			for (const str& extension : window_required)
				extension_set.emplace(extension);

			con::vector<str> extensions;
			for (const str& extension : extension_set)
				extensions.emplace_back(extension);

			return extensions;
		}

		con::vector<str> GetRequiredInstanceLayerNames() const
		{
			con::vector<str> layers;

#if DEBUG // TODO: we need a better way to clean up our debug messenger, etc - add some methods to organize this behavior ...
		  // how??
			layers.emplace_back("VK_LAYER_KHRONOS_validation");
#endif

			return layers;
		}

		VkApplicationInfo CreateVkApplicationInfo()
		{
			VkApplicationInfo info{};
			info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			info.pApplicationName = "NP Engine Application - Vulkan Instance";
			info.applicationVersion = VK_MAKE_VERSION(1, 2, 3); // TODO: get version from cmake
			info.pEngineName = "NP Engine";
			info.engineVersion = VK_MAKE_VERSION(2, 3, 4); // TODO: get version from cmake
			info.apiVersion = REQUIRED_VERSION; //minimum version we want to run on
			return info;
		}

		VkDebugUtilsMessengerCreateInfoEXT CreateVkDebugMessagerInfo()
		{
			VkDebugUtilsMessengerCreateInfoEXT info{};
			info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			info.pfnUserCallback = VulkanDebugCallback;
			info.pUserData = this;
			return info;
		}

		VkInstanceCreateInfo CreateVkInfo()
		{
			VkInstanceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

#if NP_ENGINE_PLATFORM_IS_APPLE
			info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

			return info;
		}

		VkInstance CreateVkInstance()
		{
			VkApplicationInfo app_info = CreateVkApplicationInfo();
			VkDebugUtilsMessengerCreateInfoEXT debug_msgr_info = CreateVkDebugMessagerInfo();

			con::vector<str> required_extensions = GetRequiredInstanceExtensionNames();
			con::vector<const chr*> required_extension_names;
			for (const str& extension : required_extensions)
				required_extension_names.emplace_back(extension.c_str());

			con::vector<str> required_layers = GetRequiredInstanceLayerNames();
			con::vector<const chr*> required_layer_names;
			for (const str& layer : required_layers)
				required_layer_names.emplace_back(layer.c_str());

			VkInstanceCreateInfo instance_info = CreateVkInfo();
			instance_info.pApplicationInfo = &app_info;
			instance_info.enabledExtensionCount = required_extension_names.size();
			instance_info.ppEnabledExtensionNames = required_extension_names.data();
			instance_info.enabledLayerCount = required_layer_names.size();
			instance_info.ppEnabledLayerNames = required_layer_names.data();
			instance_info.pNext = &debug_msgr_info;

			con::uset<str> required_extension_set(required_extensions.begin(), required_extensions.end());
			con::vector<str> supported_extensions = GetSupportedInstanceExtensionNames();
			for (const str& e : supported_extensions)
				required_extension_set.erase(e);

			con::uset<str> required_layer_set(required_layers.begin(), required_layers.end());
			con::vector<str> supported_layers = GetSupportedInstanceLayerNames();
			for (const str& l : supported_layers)
				required_layer_set.erase(l);

			bl layers_and_extensions_found = required_extension_set.empty() && required_layer_set.empty();

			VkInstance instance = nullptr;
			if (!layers_and_extensions_found || vkCreateInstance(&instance_info, _allocation_callbacks, &instance) != VK_SUCCESS)
				instance = nullptr;

			return instance;
		}

		VkDebugUtilsMessengerEXT CreateVkDebugMessenger()
		{
			VkDebugUtilsMessengerEXT messenger = nullptr;

			if (_instance)
			{
				using F = PFN_vkCreateDebugUtilsMessengerEXT;
				F func = (F)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");

				if (func)
				{
					VkDebugUtilsMessengerCreateInfoEXT debug_msgr_info = CreateVkDebugMessagerInfo();
					func(_instance, &debug_msgr_info, nullptr, &messenger);
				}
			}

			return messenger;
		}

	public:
		VulkanInstance(mem::sptr<srvc::Services> services):
			_services(services),
			_allocation_callbacks(_services),
			_instance(CreateVkInstance()),
			_debug_messenger(CreateVkDebugMessenger())
		{}

		virtual ~VulkanInstance()
		{
			if (_debug_messenger)
			{
				using F = PFN_vkDestroyDebugUtilsMessengerEXT;
				F func = (F)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");

				if (func)
				{
					func(_instance, _debug_messenger, nullptr);
					_debug_messenger = nullptr;
				}
			}

			if (_instance)
			{
				vkDestroyInstance(_instance, _allocation_callbacks);
				_instance = nullptr;
			}
		}

		operator VkInstance() const
		{
			return _instance;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _services;
		}

		virtual const VulkanAllocationCallbacks& GetVulkanAllocationCallbacks() const
		{
			return _allocation_callbacks;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_INSTANCE_HPP */