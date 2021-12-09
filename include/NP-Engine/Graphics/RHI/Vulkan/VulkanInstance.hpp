//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/5/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_INSTANCE_HPP
#define NP_ENGINE_VULKAN_INSTANCE_HPP

#include <iostream> //TODO: remove

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Platform/Platform.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"
#include "NP-Engine/Vendor/GlfwInclude.hpp"

namespace np::graphics::rhi
{
	class VulkanInstance
	{
	private:
		constexpr static ui32 REQUIRED_VERSION = VK_MAKE_API_VERSION(0, 1, 2, 189);

		VkInstance _instance;
		VkDebugUtilsMessengerEXT _debug_messenger;

		static VKAPI_ATTR VkBool32 VKAPI_CALL _vk_DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msg_severity,
																VkDebugUtilsMessageTypeFlagsEXT msg_type,
																const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
																void* user_data)
		{
			VkBool32 retval = VK_TRUE;
			if ((msg_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) == 0)
			{
				::std::cerr << "Validation Layer: " << callback_data->pMessage << ::std::endl;
				retval = VK_FALSE;
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
			// return VK_HEADER_VERSION_COMPLETE; //1.2.189.0
			ui32 version = 0;
			vkEnumerateInstanceVersion(&version); // version: 4202658, 1.2.162.0
			return version;
		}

		container::vector<VkExtensionProperties> GetSupportedInstanceExtensions() const
		{
			ui32 count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
			container::vector<VkExtensionProperties> extensions(count);
			vkEnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
			return extensions;
		}

		container::vector<str> GetSupportedInstanceExtensionNames() const
		{
			container::vector<VkExtensionProperties> extensions = GetSupportedInstanceExtensions();
			container::vector<str> names;
			for (auto e : extensions)
				names.emplace_back(e.extensionName);
			return names;
		}

		container::vector<VkLayerProperties> GetSupportedInstanceLayers() const
		{
			ui32 count;
			vkEnumerateInstanceLayerProperties(&count, nullptr);
			container::vector<VkLayerProperties> layers(count);
			vkEnumerateInstanceLayerProperties(&count, layers.data());
			return layers;
		}

		container::vector<str> GetSupportedInstanceLayerNames() const
		{
			container::vector<VkLayerProperties> layers = GetSupportedInstanceLayers();
			container::vector<str> names;
			for (auto e : layers)
				names.emplace_back(e.layerName);
			return names;
		}

		container::vector<str> GetRequiredInstanceExtensionNames() const
		{
			container::uset<str> extension_set;

			ui32 count;
			const chr** glfw_required = glfwGetRequiredInstanceExtensions(&count);

			for (ui32 i = 0; i < count; i++)
				extension_set.emplace(glfw_required[i]);

#if NP_ENGINE_PLATFORM_IS_APPLE
			// Apple uses MoltenVK, hence insert VK_KHR_portability_subset
			extension_set.emplace("VK_KHR_portability_subset");
#endif

			extension_set.emplace(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			container::vector<str> extensions;
			for (const str& extension : extension_set)
				extensions.emplace_back(extension);

			return extensions;
		}

		container::vector<str> GetRequiredInstanceLayerNames() const
		{
			container::vector<str> layers;

#if DEBUG // TODO: we need a better way to clean up our debug messenger, etc - add some methods to organize this behavior
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

		VkDebugUtilsMessengerCreateInfoEXT CreateDebugMessagerCreateInfo()
		{
			VkDebugUtilsMessengerCreateInfoEXT info{};
			info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			info.pfnUserCallback = _vk_DebugCallback;
			info.pUserData = this;
			return info;
		}

		VkInstanceCreateInfo CreateInstanceCreateInfo()
		{
			VkInstanceCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			return info;
		}

		VkInstance CreateInstance()
		{
			VkInstance instance = nullptr;

			VkApplicationInfo app_info = CreateApplicationInfo();
			VkDebugUtilsMessengerCreateInfoEXT debug_msgr_create_info = CreateDebugMessagerCreateInfo();

			container::vector<str> required_extensions = GetRequiredInstanceExtensionNames();
			container::vector<const chr*> required_extension_names;
			for (const str& extension : required_extensions)
				required_extension_names.emplace_back(extension.c_str());

			container::vector<str> required_layers = GetRequiredInstanceLayerNames();
			container::vector<const chr*> required_layer_names;
			for (const str& layer : required_layers)
				required_layer_names.emplace_back(layer.c_str());

			VkInstanceCreateInfo instance_create_info = CreateInstanceCreateInfo();
			instance_create_info.pApplicationInfo = &app_info;
			instance_create_info.enabledExtensionCount = required_extension_names.size();
			instance_create_info.ppEnabledExtensionNames = required_extension_names.data();
			instance_create_info.enabledLayerCount = required_layer_names.size();
			instance_create_info.ppEnabledLayerNames = required_layer_names.data();
			instance_create_info.pNext = &debug_msgr_create_info;

			container::uset<str> required_extension_set(required_extensions.begin(), required_extensions.end());
			container::vector<str> supported_extensions = GetSupportedInstanceExtensionNames();
			for (const str& e : supported_extensions)
				required_extension_set.erase(e);

			container::uset<str> required_layer_set(required_layers.begin(), required_layers.end());
			container::vector<str> supported_layers = GetSupportedInstanceLayerNames();
			for (const str& l : supported_layers)
				required_layer_set.erase(l);

			bl layers_and_extensions_found = required_extension_set.empty() && required_layer_set.empty();

			if (!layers_and_extensions_found || vkCreateInstance(&instance_create_info, nullptr, &instance) != VK_SUCCESS)
			{
				instance = nullptr;
			}

			return instance;
		}

		VkDebugUtilsMessengerEXT CreateDebugMessenger()
		{
			VkDebugUtilsMessengerEXT messenger = nullptr;

			if (_instance != nullptr)
			{
				using F = PFN_vkCreateDebugUtilsMessengerEXT;
				F func = (F)vkGetInstanceProcAddr(_instance, "vkCreateDebugUtilsMessengerEXT");

				if (func != nullptr)
				{
					VkDebugUtilsMessengerCreateInfoEXT debug_msgr_create_info = CreateDebugMessagerCreateInfo();
					func(_instance, &debug_msgr_create_info, nullptr, &messenger);
				}
			}

			return messenger;
		}

	public:
		VulkanInstance(): _instance(CreateInstance()), _debug_messenger(CreateDebugMessenger()) {}

		~VulkanInstance()
		{
			if (_debug_messenger != nullptr)
			{
				auto func =
					(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(_instance, "vkDestroyDebugUtilsMessengerEXT");

				if (func != nullptr)
				{
					func(_instance, _debug_messenger, nullptr);
				}
			}

			if (_instance != nullptr)
			{
				vkDestroyInstance(_instance, nullptr);
			}
		}

		VkInstance GetVkInstance()
		{
			return _instance;
		}

		VkInstance GetVkInstance() const
		{
			return _instance;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_INSTANCE_HPP */