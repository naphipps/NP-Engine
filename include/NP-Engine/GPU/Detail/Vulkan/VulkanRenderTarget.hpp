//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/5/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_TARGET_HPP
#define NP_ENGINE_VULKAN_RENDER_TARGET_HPP

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "NP-Engine/Vendor/GlfwInclude.hpp"

#include "VulkanInstance.hpp"

namespace np::gpu::__detail
{
	class VulkanRenderTarget : public RenderTarget
	{
	private:
		VkSurfaceKHR _surface;
		VkExtent2D _framebuffer_extent;

		static void FramebufferCallback(void* caller, ui32 width, ui32 height)
		{
			((VulkanRenderTarget*)caller)->_framebuffer_extent = { width, height };
		}

		VkSurfaceKHR CreateSurface()
		{
			VulkanInstance& instance = (VulkanInstance&)(*_instance);
			VkSurfaceKHR surface = nullptr;

			switch (_window->GetDetailType())
			{
			case win::WindowDetailType::Glfw:
				if (glfwCreateWindowSurface(instance, (GLFWwindow*)_window->GetDetailWindow(), nullptr, &surface) != VK_SUCCESS)
					surface = nullptr;
				break;

			default:
				break;
			}

			return surface;
		}

	public:

		VulkanRenderTarget(mem::sptr<DetailInstance> instance, mem::sptr<win::Window> window): 
			RenderTarget(instance, window), 
			_surface(CreateSurface())
		{
			//TODO: bring the following checks to all the detail's constructors
			NP_ENGINE_ASSERT(GetDetailType() == DetailType::Vulkan,
				"VulkanRenderTarget requires given DetailInstance to be DetailType::Vulkan");

			::glm::uvec2 framebuffer_size = _window->GetFramebufferSize();
			_framebuffer_extent = { framebuffer_size.x, framebuffer_size.y };
			_window->SetFramebufferCallback(this, FramebufferCallback);
		}

		~VulkanRenderTarget()
		{
			if (_surface)
			{
				VulkanInstance& instance = (VulkanInstance&)(*_instance);
				vkDestroySurfaceKHR(instance, _surface, nullptr);
				_surface = nullptr;
			}
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
}

#endif /* NP_ENGINE_VULKAN_RENDER_TARGET_HPP */