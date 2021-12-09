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

			if (_instance.GetVkInstance() != nullptr)
			{
				if (glfwCreateWindowSurface(_instance.GetVkInstance(), (GLFWwindow*)_window.GetNativeWindow(), nullptr,
											&surface) != VK_SUCCESS)
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
				vkDestroySurfaceKHR(_instance.GetVkInstance(), _surface, nullptr);
			}
		}

		VulkanInstance& GetInstance()
		{
			return _instance;
		}

		VulkanInstance& GetInstance() const
		{
			return _instance;
		}

		VkSurfaceKHR GetVkSurface()
		{
			return _surface;
		}

		VkSurfaceKHR GetVkSurface() const
		{
			return _surface;
		}

		window::Window& GetWindow()
		{
			return _window;
		}

		const window::Window& GetWindow() const
		{
			return _window;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SURFACE_HPP */