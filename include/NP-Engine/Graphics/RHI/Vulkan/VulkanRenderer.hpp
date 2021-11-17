//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_VULKAN_RENDERER_HPP
#define NP_ENGINE_GRAPHICS_VULKAN_RENDERER_HPP

#include <iostream> //TODO: remove

#include <vulkan/vulkan.hpp>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "../../RPI/Renderer.hpp"



//TODO: add summary comments

namespace np::graphics::rhi
{
	class VulkanRenderer : public Renderer
	{
	private:

		bl _is_enabled;

	public:

		constexpr static ui32 REQUIRED_VERSION = VK_MAKE_API_VERSION(0, 1, 2, 189);


		VulkanRenderer():
		_is_enabled(false)
		{
			/*
				version: 4202658
				1.2.162.0
			*/
			ui32 version = VK_HEADER_VERSION_COMPLETE;
			//vkEnumerateInstanceVersion(&version);

			std::cout << "version: " << version << "\n";
			std::cout << "\t" << VK_API_VERSION_MAJOR(version) << "." << VK_API_VERSION_MINOR(version) << "." << VK_API_VERSION_PATCH(version) << "." << VK_API_VERSION_VARIANT(version) << "\n";

			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			container::vector<VkExtensionProperties> extensionProperties(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

			std::cout << extensionCount << " extensions supported:\n";

			for (VkExtensionProperties& prop : extensionProperties)
			{
				::std::cout << "\t- " << prop.extensionName << ", " << prop.specVersion << "\n";
			}

			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			std::cout << layerCount << " layer supported\n";

			for (VkLayerProperties& prop : availableLayers)
			{
				::std::cout << "\t- " << prop.layerName << ", " << prop.description << ", " << prop.implementationVersion << ", " << prop.specVersion << "\n";
			}



		}

		Renderer::RhiType GetRhiType() const override
		{
			return Renderer::RhiType::Vulkan;
		}


		bl IsEnabled() const override
		{
			return _is_enabled;
		}

		str GetName() const override
		{
			return "Vulkan";
		}
	};
}

#endif /* NP_ENGINE_GRAPHICS_VULKAN_RENDERER_HPP */