//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDERER_HPP
#define NP_ENGINE_VULKAN_RENDERER_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanShader.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanFrame.hpp"
#include "VulkanFramebuffers.hpp"

namespace np::gfx::rhi
{
	class VulkanRenderer : public Renderer
	{
	private:
		VulkanInstance* _instance;
		VulkanSurface* _surface;
		VulkanDevice* _device;
		VulkanSwapchain* _swapchain;
		VulkanRenderPass* _render_pass;
		VulkanFramebuffers* _framebuffers;
		con::vector<VulkanCommandBuffer> _command_buffers;
		str _object_vertex_shader_filename;
		str _object_fragment_shader_filename;
		VulkanShader* _object_vertex_shader;
		VulkanShader* _object_fragment_shader;
		VulkanPipeline* _object_pipeline;
		str _light_vertex_shader_filename;
		str _light_fragment_shader_filename;
		VulkanShader* _light_vertex_shader;
		VulkanShader* _light_fragment_shader;
		VulkanPipeline* _light_pipeline;
		con::vector<VkClearValue> _clear_values;
		VkCommandBufferBeginInfo _command_buffer_begin_info;
		VkRenderPassBeginInfo _render_pass_begin_info;
		VulkanFrame _frame;
		bl _is_out_of_date;

		void AdjustForOutOfDate()
		{
			if (_device)
			{
				SetOutOfDate(false);
				vkDeviceWaitIdle(GetDevice());
				GetSwapchain().Rebuild();
				GetRenderPass().Rebuild();
				GetFramebuffers().Rebuild();
				GetObjectPipeline().Rebuild();
				GetLightPipeline().Rebuild();
				_frame.Invalidate();
			}
		}

		con::vector<VulkanCommandBuffer> CreateCommandBuffers()
		{
			VkCommandBufferAllocateInfo command_buffer_allocate_info =
				GetDevice().GetCommandPool().CreateCommandBufferAllocateInfo();
			command_buffer_allocate_info.commandPool = GetDevice().GetCommandPool();
			command_buffer_allocate_info.commandBufferCount = NP_ENGINE_VULKAN_MAX_FRAME_COUNT;
			return GetDevice().GetCommandPool().AllocateCommandBuffers(command_buffer_allocate_info);
		}

		VkRenderPassBeginInfo CreateRenderPassBeginInfo()
		{
			VkRenderPassBeginInfo info = VulkanRenderPass::CreateRenderPassBeginInfo();
			info.clearValueCount = (ui32)_clear_values.size();
			info.pClearValues = _clear_values.data();
			return info;
		}

		void Dispose()
		{
			if (_device)
			{
				vkDeviceWaitIdle(*_device);
			}

			if (_light_pipeline)
			{
				mem::Destroy<VulkanPipeline>(_services.GetAllocator(), _light_pipeline);
				_light_pipeline = nullptr;
			}

			if (_light_fragment_shader)
			{
				mem::Destroy<VulkanShader>(_services.GetAllocator(), _light_fragment_shader);
				_light_fragment_shader = nullptr;
			}

			if (_light_vertex_shader)
			{
				mem::Destroy<VulkanShader>(_services.GetAllocator(), _light_vertex_shader);
				_light_vertex_shader = nullptr;
			}

			if (_object_pipeline)
			{
				mem::Destroy<VulkanPipeline>(_services.GetAllocator(), _object_pipeline);
				_object_pipeline = nullptr;
			}

			if (_object_fragment_shader)
			{
				mem::Destroy<VulkanShader>(_services.GetAllocator(), _object_fragment_shader);
				_object_fragment_shader = nullptr;
			}

			if (_object_vertex_shader)
			{
				mem::Destroy<VulkanShader>(_services.GetAllocator(), _object_vertex_shader);
				_object_vertex_shader = nullptr;
			}

			if (_framebuffers)
			{
				mem::Destroy<VulkanFramebuffers>(_services.GetAllocator(), _framebuffers);
				_framebuffers = nullptr;
			}

			if (_render_pass)
			{
				mem::Destroy<VulkanRenderPass>(_services.GetAllocator(), _render_pass);
				_render_pass = nullptr;
			}

			if (_swapchain)
			{
				mem::Destroy<VulkanSwapchain>(_services.GetAllocator(), _swapchain);
				_swapchain = nullptr;
			}

			if (_device)
			{
				_device->GetCommandPool().FreeCommandBuffers(_command_buffers);
				mem::Destroy<VulkanDevice>(_services.GetAllocator(), _device);
				_device = nullptr;
			}

			if (_surface)
			{
				mem::Destroy<VulkanSurface>(_services.GetAllocator(), _surface);
				_surface = nullptr;
			}

			if (_instance)
			{
				mem::Destroy<VulkanInstance>(_services.GetAllocator(), _instance);
				_instance = nullptr;
			}
		}

	public:
		VulkanRenderer(srvc::Services& services):
			Renderer(services),
			_instance(mem::Create<VulkanInstance>(_services.GetAllocator())),
			_surface(nullptr),
			_device(nullptr),
			_swapchain(nullptr),
			_render_pass(nullptr),
			_framebuffers(nullptr),
			_object_vertex_shader_filename(fs::Append(fs::Append("Vulkan", "shaders"), "object_vertex.glsl")),
			_object_fragment_shader_filename(fs::Append(fs::Append("Vulkan", "shaders"), "object_fragment.glsl")),
			_object_vertex_shader(nullptr),
			_object_fragment_shader(nullptr),
			_object_pipeline(nullptr),
			_light_vertex_shader_filename(fs::Append(fs::Append("Vulkan", "shaders"), "light_vertex.glsl")),
			_light_fragment_shader_filename(fs::Append(fs::Append("Vulkan", "shaders"), "light_fragment.glsl")),
			_light_vertex_shader(nullptr),
			_light_fragment_shader(nullptr),
			_light_pipeline(nullptr),
			_clear_values(VulkanRenderPass::CreateClearValues()),
			_command_buffer_begin_info(VulkanCommandBuffer::CreateBeginInfo()),
			_render_pass_begin_info(CreateRenderPassBeginInfo()),
			_is_out_of_date(false)
		{}

		~VulkanRenderer()
		{
			Dispose();
		}

		RhiType GetRhiType() const override
		{
			return RhiType::Vulkan;
		}

		str GetName() const override
		{
			return "Vulkan";
		}

		virtual bl IsAttachedToWindow(win::Window& window) const override
		{
			return _surface && mem::AddressOf(_surface->GetWindow()) == mem::AddressOf(window);
		}

		void AttachToWindow(win::Window& window) override
		{
			_surface = mem::Create<VulkanSurface>(_services.GetAllocator(), *_instance, window);
			_device = mem::Create<VulkanDevice>(_services.GetAllocator(), *_surface);
			_swapchain = mem::Create<VulkanSwapchain>(_services.GetAllocator(), *_device);
			_render_pass = mem::Create<VulkanRenderPass>(_services.GetAllocator(), *_swapchain);
			_framebuffers = mem::Create<VulkanFramebuffers>(_services.GetAllocator(), *_swapchain, *_render_pass);

			_command_buffers = CreateCommandBuffers();

			_object_vertex_shader = mem::Create<VulkanShader>(_services.GetAllocator(), *_device,
																 _object_vertex_shader_filename, VulkanShader::Type::VERTEX);
			_object_fragment_shader = mem::Create<VulkanShader>(
				_services.GetAllocator(), *_device, _object_fragment_shader_filename, VulkanShader::Type::FRAGMENT);
			_object_pipeline = mem::Create<VulkanPipeline>(_services.GetAllocator(), *_swapchain, *_render_pass,
															  *_object_vertex_shader, *_object_fragment_shader);

			_light_vertex_shader = mem::Create<VulkanShader>(_services.GetAllocator(), *_device,
																_light_vertex_shader_filename, VulkanShader::Type::VERTEX);
			_light_fragment_shader = mem::Create<VulkanShader>(
				_services.GetAllocator(), *_device, _light_fragment_shader_filename, VulkanShader::Type::FRAGMENT);
			_light_pipeline = mem::Create<VulkanPipeline>(_services.GetAllocator(), *_swapchain, *_render_pass,
															 *_light_vertex_shader, *_light_fragment_shader);
		}

		void DetachFromWindow(win::Window& window) override
		{
			if (IsAttachedToWindow(window))
			{
				Dispose();
				_frame.Invalidate();
			}
		}

		void BeginRenderPassOnFrame(VulkanFrame& frame)
		{
			_render_pass_begin_info.framebuffer = GetFramebuffers()[GetSwapchain().GetAcquiredImageIndex()];
			_render_pass->Begin(_render_pass_begin_info, frame);
		}

		void EndRenderPassOnFrame(VulkanFrame& frame)
		{
			_render_pass->End(frame);
		}

		void SetOutOfDate(bl out_of_date = true)
		{
			_is_out_of_date = out_of_date;
		}

		bl IsOutOfDate() const
		{
			return _is_out_of_date;
		}

		Frame& BeginFrame() override
		{
			_frame.Invalidate();
			i32 width = 0;
			i32 height = 0;
			glfwGetFramebufferSize((GLFWwindow*)GetSurface().GetWindow().GetNativeWindow(), &width, &height);
			if (width != 0 && height != 0)
			{
				VkResult result = GetSwapchain().AcquireImage();
				if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
				{
					GetSwapchain().MarkAcquiredImageForUse();
					_frame = _command_buffers[GetSwapchain().GetCurrentImageIndex()];
					_frame.Begin(_command_buffer_begin_info);
					NP_ENGINE_ASSERT(_frame.IsValid(), "frame must be valid here");
				}
				else if (result == VK_ERROR_OUT_OF_DATE_KHR)
				{
					AdjustForOutOfDate();
				}
			}

			return _frame;
		}

		void EndFrame() override
		{
			_frame.End();
		}

		void DrawFrame() override
		{
			ui32 image_index = GetSwapchain().GetAcquiredImageIndex();
			ui32 current_index = GetSwapchain().GetCurrentImageIndex();
			con::vector<VkCommandBuffer> buffers = {_command_buffers[current_index]};
			con::vector<VkSemaphore> wait_semaphores{GetSwapchain().GetImageSemaphores()[current_index]};
			con::vector<VkPipelineStageFlags> wait_stages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			con::vector<VkSemaphore> signal_semaphores{GetSwapchain().GetRenderSemaphores()[current_index]};
			con::vector<VkSwapchainKHR> swapchains{GetSwapchain()};

			VkSubmitInfo submit_info{};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.waitSemaphoreCount = wait_semaphores.size();
			submit_info.pWaitSemaphores = wait_semaphores.data();
			submit_info.pWaitDstStageMask = wait_stages.data();
			submit_info.commandBufferCount = (ui32)buffers.size();
			submit_info.pCommandBuffers = buffers.data();
			submit_info.signalSemaphoreCount = (ui32)signal_semaphores.size();
			submit_info.pSignalSemaphores = signal_semaphores.data();

			vkResetFences(GetDevice(), 1, &GetSwapchain().GetFences()[current_index]);

			if (GetDevice().GetGraphicsQueue().Submit({submit_info}, GetSwapchain().GetFences()[current_index]) != VK_SUCCESS)
			{
				NP_ENGINE_ASSERT(false, "failed to submit draw command buffer!");
			}

			VkPresentInfoKHR present_info{};
			present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present_info.waitSemaphoreCount = signal_semaphores.size();
			present_info.pWaitSemaphores = signal_semaphores.data();
			present_info.swapchainCount = swapchains.size();
			present_info.pSwapchains = swapchains.data();
			present_info.pImageIndices = &image_index;

			VkResult present_result = vkQueuePresentKHR(GetDevice().GetPresentQueue(), &present_info);

			if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR || IsOutOfDate())
			{
				AdjustForOutOfDate();
			}
			else if (present_result != VK_SUCCESS)
			{
				NP_ENGINE_ASSERT(false, "vkQueuePresentKHR error");
			}

			vkQueueWaitIdle(GetDevice().GetPresentQueue());
			GetSwapchain().IncCurrentImage();
			_frame.Invalidate();
		}

		void AdjustForWindowResize(win::Window& window) override {}

		VulkanInstance& GetInstance()
		{
			return *_instance;
		}

		const VulkanInstance& GetInstance() const
		{
			return *_instance;
		}

		VulkanSurface& GetSurface()
		{
			return *_surface;
		}

		const VulkanSurface& GetSurface() const
		{
			return *_surface;
		}

		VulkanDevice& GetDevice()
		{
			return *_device;
		}

		const VulkanDevice& GetDevice() const
		{
			return *_device;
		}

		VulkanSwapchain& GetSwapchain()
		{
			return *_swapchain;
		}

		const VulkanSwapchain& GetSwapchain() const
		{
			return *_swapchain;
		}

		VulkanRenderPass& GetRenderPass()
		{
			return *_render_pass;
		}

		const VulkanRenderPass& GetRenderPass() const
		{
			return *_render_pass;
		}

		VulkanFramebuffers& GetFramebuffers()
		{
			return *_framebuffers;
		}

		const VulkanFramebuffers& GetFramebuffers() const
		{
			return *_framebuffers;
		}

		VulkanPipeline& GetObjectPipeline()
		{
			return *_object_pipeline;
		}

		const VulkanPipeline& GetObjectPipeline() const
		{
			return *_object_pipeline;
		}

		VulkanPipeline& GetLightPipeline()
		{
			return *_light_pipeline;
		}

		const VulkanPipeline& GetLightPipeline() const
		{
			return *_light_pipeline;
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_RENDERER_HPP */