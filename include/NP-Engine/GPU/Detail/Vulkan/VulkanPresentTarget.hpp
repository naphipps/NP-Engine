//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/5/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_PRESENT_TARGET_HPP
#define NP_ENGINE_GPU_VULKAN_PRESENT_TARGET_HPP

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "NP-Engine/Vendor/GlfwInclude.hpp"

#include "VulkanInstance.hpp"

namespace np::gpu::__detail
{
	class VulkanPresentTarget : public PresentTarget
	{
	private:
		mem::sptr<VulkanInstance> _instance;
		mem::sptr<win::Window> _window;
		VkSurfaceKHR _surface;
		VkExtent2D _framebuffer_extent;

		static void FramebufferCallback(void* caller, ui32 width, ui32 height)
		{
			// NVIDIA resizing framebuffer is a nightmare //TODO: is this still needed?
			static_cast<VulkanPresentTarget*>(caller)->_framebuffer_extent = {width, height};
		}

		VkSurfaceKHR CreateVkSurface()
		{
			VkSurfaceKHR surface = nullptr;

			switch (_window->GetDetailType())
			{
			case win::DetailType::Glfw:
				if (glfwCreateWindowSurface(*_instance, (GLFWwindow*)_window->GetDetailWindow(), nullptr, &surface) != VK_SUCCESS)
					surface = nullptr;
				break;

			default:
				break;
			}

			return surface;
		}

	public:
		VulkanPresentTarget(mem::sptr<DetailInstance> instance, mem::sptr<win::Window> window) :
			_instance(DetailObject::EnsureIsDetailType(instance, DetailType::Vulkan)),
			_window(window),
			_surface(CreateVkSurface())
		{
			::glm::uvec2 framebuffer_size = _window->GetFramebufferSize();
			_framebuffer_extent = {framebuffer_size.x, framebuffer_size.y};
			_window->SetFramebufferCallback(this, FramebufferCallback);
		}

		virtual ~VulkanPresentTarget()
		{
			if (_surface)
			{
				vkDestroySurfaceKHR(*_instance, _surface, nullptr);
				_surface = nullptr;
			}
		}

		operator VkSurfaceKHR() const
		{
			return _surface;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<DetailInstance> GetDetailInstance() const override
		{
			return _instance;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _instance->GetServices();
		}

		virtual mem::sptr<win::Window> GetWindow() const override
		{
			return _window;
		}

		VkExtent2D GetFramebufferExtent() const //TODO: do we want to rename this to GetExtent? seems straight forward enough to me
		{
			return _framebuffer_extent;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_PRESENT_TARGET_HPP */