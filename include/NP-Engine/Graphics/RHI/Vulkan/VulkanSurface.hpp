//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/5/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SURFACE_HPP
#define NP_ENGINE_VULKAN_SURFACE_HPP

#include "NP-Engine/Window/Window.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"
#include "NP-Engine/Vendor/GlfwInclude.hpp"

#include "VulkanInstance.hpp"

namespace np::graphics::rhi
{
	class VulkanSurface
	{
	private:
		VulkanInstance& _instance;
		window::Window& _window;
		VkSurfaceKHR _surface;

		VkSurfaceKHR CreateSurface()
		{
			VkSurfaceKHR surface = nullptr;

			if ((VkInstance)_instance != nullptr)
			{
				if (glfwCreateWindowSurface(_instance, (GLFWwindow*)_window.GetNativeWindow(), nullptr, &surface) != VK_SUCCESS)
				{
					surface = nullptr;
				}
			}

			return surface;
		}

	public:
		VulkanSurface(VulkanInstance& instance, window::Window& window):
			_instance(instance),
			_window(window),
			_surface(CreateSurface())
		{}

		~VulkanSurface()
		{
			if (_surface != nullptr)
			{
				vkDestroySurfaceKHR(_instance, _surface, nullptr);
			}
		}

		VulkanInstance& Instance() const
		{
			return _instance;
		}

		const window::Window& Window() const
		{
			return _window;
		}

		operator VkSurfaceKHR() const
		{
			return _surface;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SURFACE_HPP */