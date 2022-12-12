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

namespace np::gfx::rhi
{
	class VulkanSurface
	{
	private:
		VulkanInstance& _instance;
		win::Window& _window;
		VkSurfaceKHR _surface;
		VkExtent2D _framebuffer_extent;

		static void FramebufferCallback(void* caller, ui32 width, ui32 height)
		{
			VulkanSurface* surface = (VulkanSurface*)caller;
			surface->_framebuffer_extent = { width, height };
		}

		VkSurfaceKHR CreateSurface()
		{
			VkSurfaceKHR surface = nullptr;

			switch (win::__detail::RegisteredWindowDetailType.load(mo_acquire))
			{
			case win::WindowDetailType::Glfw:
				if ((VkInstance)_instance)
					if (glfwCreateWindowSurface(_instance, (GLFWwindow*)_window.GetDetailWindow(), nullptr, &surface) != VK_SUCCESS)
						surface = nullptr;
				break;

			default:
				break;
			}

			return surface;
		}

	public:
		VulkanSurface(VulkanInstance& instance, win::Window& window):
			_instance(instance),
			_window(window),
			_surface(CreateSurface())
		{
			::glm::uvec2 framebuffer_size = _window.GetFramebufferSize();
			_framebuffer_extent.width = framebuffer_size.x;
			_framebuffer_extent.height = framebuffer_size.y;
			_window.SetFramebufferCallback(this, FramebufferCallback);
		}

		~VulkanSurface()
		{
			if (_surface)
			{
				vkDestroySurfaceKHR(_instance, _surface, nullptr);
				_surface = nullptr;
			}
		}

		VulkanInstance& GetInstance() const
		{
			return _instance;
		}

		win::Window& GetWindow() const
		{
			return _window;
		}

		VkExtent2D GetFramebufferExtent() const
		{
			return _framebuffer_extent;
		}

		operator VkSurfaceKHR() const
		{
			return _surface;
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_SURFACE_HPP */