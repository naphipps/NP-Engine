//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/5/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_INSTANCE_HPP
#define NP_ENGINE_VULKAN_INSTANCE_HPP

#include <iostream>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Platform/Platform.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

namespace np::gfx::rhi
{
	class VulkanInstance
	{
	private:
		constexpr static ui32 REQUIRED_VERSION = VK_MAKE_API_VERSION(0, 1, 2, 189);

		VkInstance _instance;
		VkDebugUtilsMessengerEXT _debug_messenger;

		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
																  VkDebugUtilsMessageTypeFlagsEXT msg_type,
																  const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
																  void* user_data)
		{
			// TODO: feel like we should pipe this stuff through our logger
			// TODO: should we have a macro to enable this? NP_ENGINE_ENABLE_VULKAN_DEBUG_CALLBACK?

			con::vector<str> known_msgs{
				R"(loader_scanned_icd_add: Driver C:\Windows\System32\DriverStore\FileRepository\u0377495.inf_amd64_58cc395c0bf03a26\B377432\.\amdvlk64.dll says it supports interface version 6 but still exports core entrypoints (Policy #LDP_DRIVER_6))" // vulkansdk 1.3.211
			};

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
					str msg = "\nNP Validation Layer: ";
					msg += "[[" + to_str(callback_data->pMessageIdName) + ", " + to_str(callback_data->messageIdNumber) + "]] ";
					msg += to_str(callback_data->pMessage);
					msg += "\n";

					::std::cerr << msg;
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

		con::vector<VkExtensionProperties> GetSupportedInstanceExtensions() const
		{
			ui32 count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
			con::vector<VkExtensionProperties> extensions(count);
			vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
			return extensions;
		}

		con::vector<str> GetSupportedInstanceExtensionNames() const
		{
			con::vector<VkExtensionProperties> extensions = GetSupportedInstanceExtensions();
			con::vector<str> names;
			for (auto e : extensions)
				names.emplace_back(e.extensionName);
			return names;
		}

		con::vector<VkLayerProperties> GetSupportedInstanceLayers() const
		{
			ui32 count;
			vkEnumerateInstanceLayerProperties(&count, nullptr);
			con::vector<VkLayerProperties> layers(count);
			vkEnumerateInstanceLayerProperties(&count, layers.data());
			return layers;
		}

		con::vector<str> GetSupportedInstanceLayerNames() const
		{
			con::vector<VkLayerProperties> layers = GetSupportedInstanceLayers();
			con::vector<str> names;
			for (auto e : layers)
				names.emplace_back(e.layerName);
			return names;
		}

		con::vector<str> GetRequiredInstanceExtensionNames() const
		{
			con::uset<str> extension_set;
			extension_set.emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			con::vector<str> window_required = win::Window::GetRequiredGfxExtentions();
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

		VkApplicationInfo CreateApplicationInfo()
		{
			VkApplicationInfo info{};
			info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			info.pApplicationName = "NP Engine Application - Vulkan Instance";
			info.applicationVersion = VK_MAKE_VERSION(1, 2, 3); // TODO: get version from cmake
			info.pEngineName = "NP Engine";
			info.engineVersion = VK_MAKE_VERSION(2, 3, 4); // TODO: get version from cmake
			info.apiVersion = REQUIRED_VERSION;
			return info;
		}

		VkDebugUtilsMessengerCreateInfoEXT CreateDebugMessagerInfo()
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

		VkInstanceCreateInfo CreateInstanceInfo()
		{
			VkInstanceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			return info;
		}

		VkInstance CreateInstance()
		{
			VkInstance instance = nullptr;

			VkApplicationInfo app_info = CreateApplicationInfo();
			VkDebugUtilsMessengerCreateInfoEXT debug_msgr_info = CreateDebugMessagerInfo();

			con::vector<str> required_extensions = GetRequiredInstanceExtensionNames();
			con::vector<const chr*> required_extension_names;
			for (const str& extension : required_extensions)
				required_extension_names.emplace_back(extension.c_str());

			con::vector<str> required_layers = GetRequiredInstanceLayerNames();
			con::vector<const chr*> required_layer_names;
			for (const str& layer : required_layers)
				required_layer_names.emplace_back(layer.c_str());

			VkInstanceCreateInfo instance_info = CreateInstanceInfo();
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

			if (!layers_and_extensions_found || vkCreateInstance(&instance_info, nullptr, &instance) != VK_SUCCESS)
			{
				instance = nullptr;
			}

			return instance;
		}

		VkDebugUtilsMessengerEXT CreateDebugMessenger()
		{
			VkDebugUtilsMessengerEXT messenger = nullptr;

			if (_instance)
			{
				using F = PFN_vkCreateDebugUtilsMessengerEXT;
				F func = (F)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");

				if (func)
				{
					VkDebugUtilsMessengerCreateInfoEXT debug_msgr_info = CreateDebugMessagerInfo();
					func(_instance, &debug_msgr_info, nullptr, &messenger);
				}
			}

			return messenger;
		}

	public:
		VulkanInstance(): _instance(CreateInstance()), _debug_messenger(CreateDebugMessenger()) {}

		~VulkanInstance()
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
				vkDestroyInstance(_instance, nullptr);
				_instance = nullptr;
			}
		}

		operator VkInstance() const
		{
			return _instance;
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_INSTANCE_HPP */