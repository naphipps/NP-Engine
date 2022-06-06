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

#include "NP-Engine/Vendor/VulkanInclude.hpp"
#include "NP-Engine/Vendor/EnttInclude.hpp"

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

namespace np::graphics::rhi
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
		container::vector<VulkanCommandBuffer> _command_buffers;
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
		container::vector<VkClearValue> _clear_values;
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
				GetDevice().GetCommandPool().FreeCommandBuffers(_command_buffers);
				_command_buffers = CreateCommandBuffers();
				GetObjectPipeline().Rebuild();
				GetLightPipeline().Rebuild();
				_frame.Invalidate();
			}
		}

		container::vector<VulkanCommandBuffer> CreateCommandBuffers()
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

	public:
		VulkanRenderer(::entt::registry& ecs_registry):
			Renderer(ecs_registry),
			_instance(memory::Create<VulkanInstance>(memory::DefaultTraitAllocator)),
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
			if (_device)
				vkDeviceWaitIdle(*_device);

			if (_light_pipeline)
				memory::Destroy<VulkanPipeline>(memory::DefaultTraitAllocator, _light_pipeline);

			if (_light_fragment_shader)
				memory::Destroy<VulkanShader>(memory::DefaultTraitAllocator, _light_fragment_shader);

			if (_light_vertex_shader)
				memory::Destroy<VulkanShader>(memory::DefaultTraitAllocator, _light_vertex_shader);

			if (_object_pipeline)
				memory::Destroy<VulkanPipeline>(memory::DefaultTraitAllocator, _object_pipeline);

			if (_object_fragment_shader)
				memory::Destroy<VulkanShader>(memory::DefaultTraitAllocator, _object_fragment_shader);

			if (_object_vertex_shader)
				memory::Destroy<VulkanShader>(memory::DefaultTraitAllocator, _object_vertex_shader);

			if (_framebuffers)
				memory::Destroy<VulkanFramebuffers>(memory::DefaultTraitAllocator, _framebuffers);

			if (_render_pass)
				memory::Destroy<VulkanRenderPass>(memory::DefaultTraitAllocator, _render_pass);

			if (_swapchain)
				memory::Destroy<VulkanSwapchain>(memory::DefaultTraitAllocator, _swapchain);

			if (_device)
			{
				_device->GetCommandPool().FreeCommandBuffers(_command_buffers);
				memory::Destroy<VulkanDevice>(memory::DefaultTraitAllocator, _device);
			}

			if (_surface)
				memory::Destroy<VulkanSurface>(memory::DefaultTraitAllocator, _surface);

			if (_instance)
				memory::Destroy<VulkanInstance>(memory::DefaultTraitAllocator, _instance);
		}

		RhiType GetRhiType() const override
		{
			return RhiType::Vulkan;
		}

		str GetName() const override
		{
			return "Vulkan";
		}

		virtual bl IsAttachedToWindow(window::Window& window) const override
		{
			return _surface && memory::AddressOf(_surface->GetWindow()) == memory::AddressOf(window);
		}

		void AttachToWindow(window::Window& window) override
		{
			_surface = memory::Create<VulkanSurface>(memory::DefaultTraitAllocator, *_instance, window);
			_device = memory::Create<VulkanDevice>(memory::DefaultTraitAllocator, *_surface);
			_swapchain = memory::Create<VulkanSwapchain>(memory::DefaultTraitAllocator, *_device);
			_render_pass = memory::Create<VulkanRenderPass>(memory::DefaultTraitAllocator, *_swapchain);
			_framebuffers = memory::Create<VulkanFramebuffers>(memory::DefaultTraitAllocator, *_swapchain, *_render_pass);

			_command_buffers = CreateCommandBuffers();

			_object_vertex_shader = memory::Create<VulkanShader>(memory::DefaultTraitAllocator, *_device,
																 _object_vertex_shader_filename, VulkanShader::Type::VERTEX);
			_object_fragment_shader = memory::Create<VulkanShader>(
				memory::DefaultTraitAllocator, *_device, _object_fragment_shader_filename, VulkanShader::Type::FRAGMENT);
			_object_pipeline = memory::Create<VulkanPipeline>(memory::DefaultTraitAllocator, *_swapchain, *_render_pass,
															  *_object_vertex_shader, *_object_fragment_shader);

			_light_vertex_shader = memory::Create<VulkanShader>(memory::DefaultTraitAllocator, *_device,
																_light_vertex_shader_filename, VulkanShader::Type::VERTEX);
			_light_fragment_shader = memory::Create<VulkanShader>(
				memory::DefaultTraitAllocator, *_device, _light_fragment_shader_filename, VulkanShader::Type::FRAGMENT);
			_light_pipeline = memory::Create<VulkanPipeline>(memory::DefaultTraitAllocator, *_swapchain, *_render_pass,
															 *_light_vertex_shader, *_light_fragment_shader);
		}

		void DetachFromWindow(window::Window& window) override
		{
			if (IsAttachedToWindow(window))
			{
				vkDeviceWaitIdle(*_device);
				memory::Destroy<VulkanPipeline>(memory::DefaultTraitAllocator, _light_pipeline);
				memory::Destroy<VulkanShader>(memory::DefaultTraitAllocator, _light_vertex_shader);
				memory::Destroy<VulkanShader>(memory::DefaultTraitAllocator, _light_fragment_shader);
				memory::Destroy<VulkanPipeline>(memory::DefaultTraitAllocator, _object_pipeline);
				memory::Destroy<VulkanShader>(memory::DefaultTraitAllocator, _object_vertex_shader);
				memory::Destroy<VulkanShader>(memory::DefaultTraitAllocator, _object_fragment_shader);
				memory::Destroy<VulkanFramebuffers>(memory::DefaultTraitAllocator, _framebuffers);
				memory::Destroy<VulkanRenderPass>(memory::DefaultTraitAllocator, _render_pass);
				memory::Destroy<VulkanSwapchain>(memory::DefaultTraitAllocator, _swapchain);
				memory::Destroy<VulkanDevice>(memory::DefaultTraitAllocator, _device);
				memory::Destroy<VulkanSurface>(memory::DefaultTraitAllocator, _surface);
				_light_pipeline = nullptr;
				_light_vertex_shader = nullptr;
				_light_fragment_shader = nullptr;
				_object_pipeline = nullptr;
				_object_vertex_shader = nullptr;
				_object_fragment_shader = nullptr;
				_framebuffers = nullptr;
				_render_pass = nullptr;
				_swapchain = nullptr;
				_device = nullptr;
				_surface = nullptr;
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
			container::vector<VkCommandBuffer> buffers = {_command_buffers[current_index]};
			container::vector<VkSemaphore> wait_semaphores{GetSwapchain().GetImageSemaphores()[current_index]};
			container::vector<VkPipelineStageFlags> wait_stages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			container::vector<VkSemaphore> signal_semaphores{GetSwapchain().GetRenderSemaphores()[current_index]};
			container::vector<VkSwapchainKHR> swapchains{GetSwapchain()};

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

			if (vkQueueSubmit(GetDevice().GetGraphicsDeviceQueue(), 1, &submit_info,
							  GetSwapchain().GetFences()[current_index]) != VK_SUCCESS)
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

			VkResult present_result = vkQueuePresentKHR(GetDevice().GetPresentDeviceQueue(), &present_info);

			if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR || IsOutOfDate())
			{
				AdjustForOutOfDate();
			}
			else if (present_result != VK_SUCCESS)
			{
				NP_ENGINE_ASSERT(false, "vkQueuePresentKHR error");
			}

			vkQueueWaitIdle(GetDevice().GetPresentDeviceQueue());
			GetSwapchain().IncCurrentImage();
			_frame.Invalidate();
		}

		void AdjustForWindowResize(window::Window& window) override {}

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
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_RENDERER_HPP */