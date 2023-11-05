//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/16/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SCENE_HPP
#define NP_ENGINE_VULKAN_SCENE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanRenderableModel.hpp"
#include "VulkanRenderPipeline.hpp"

namespace np::gpu::__detail
{
	class VulkanScene : public Scene
	{
	private:
		VkCommandBufferBeginInfo _command_buffer_begin_info;

		con::vector<VkClearValue> _clear_values;
		VkRenderPassBeginInfo _render_pass_begin_info;
		bl _is_out_of_date;

		static VkRenderPassBeginInfo CreateRenderPassBeginInfo(con::vector<VkClearValue>& clear_values)
		{
			VkRenderPassBeginInfo info = VulkanRenderPass::CreateRenderPassBeginInfo();
			info.clearValueCount = (ui32)clear_values.size();
			info.pClearValues = clear_values.data();
			return info;
		}

		void AdjustForOutOfDate()
		{
			VulkanRenderPipeline& vulkan_render_pipeline = (VulkanRenderPipeline&)(*GetRenderPipeline());
			VulkanFramebuffers& vulkan_framebuffers = (VulkanFramebuffers&)(*GetRenderPipeline()->GetProperties().framebuffers);
			VulkanRenderPass& vulkan_render_pass = (VulkanRenderPass&)(*vulkan_framebuffers.GetRenderPass());
			VulkanRenderContext& vulkan_render_context = (VulkanRenderContext&)(*GetRenderContext());
			VulkanRenderDevice& vulkan_render_device = (VulkanRenderDevice&)(*GetRenderDevice());

			SetOutOfDate(false);
			vkDeviceWaitIdle(vulkan_render_device);
			vulkan_render_context.Rebuild();
			vulkan_render_pass.Rebuild();
			vulkan_framebuffers.Rebuild();
			vulkan_render_pipeline.Rebuild();
		}

		/* TODO: 
		void StageResource(mem::sptr<Resource> resource, Scene::RenderVisiblePayload& payload) override
		{
			switch (resource->GetType())
			{
			case ResourceType::RenderableModel:
				StageRenderableModel((VulkanRenderableModel&)(*resource), payload);
				break;

			default:
				break;
			}
		}

		void StageRenderableModel(VulkanRenderableModel& renderable_model, Scene::RenderVisiblePayload& payload)
		{
			renderable_model.Stage(payload.scenePayload->staging, _properties.renderPipeline);
		}
		//*/

	public:
		VulkanScene(Scene::Properties& properties):
			Scene(properties),
			_command_buffer_begin_info(VulkanCommandBuffer::CreateBeginInfo()),
			_clear_values(VulkanRenderPass::CreateClearValues()),
			_render_pass_begin_info(CreateRenderPassBeginInfo(_clear_values)),
			_is_out_of_date(false)
		{}

		~VulkanScene() = default;

		mem::sptr<Resource> CreateResource(mem::sptr<Model> model) override // TODO: for image and light too
		{
			// TODO: consider an accumulating pool
			return mem::create_sptr<VulkanRenderableModel>(GetServices()->GetAllocator(), GetServices(), model);
		}

		void UpdateCamera()
		{
			VulkanRenderContext& vulkan_render_context = (VulkanRenderContext&)(*GetRenderContext());
			VkExtent2D extent = vulkan_render_context.GetExtent();
			_properties.camera.aspectRatio = (flt)extent.width / (flt)extent.height;
			_properties.camera.leftPlane = -(flt)extent.width / 2.f;
			_properties.camera.rightPlane = (flt)extent.width / 2.f;
			_properties.camera.bottomPlane = -(flt)extent.height / 2.f;
			_properties.camera.topPlane = (flt)extent.height / 2.f;
			_properties.camera.UpdateViewAndProjection();
		}

		void SetOutOfDate(bl out_of_date = true)
		{
			_is_out_of_date = out_of_date;
		}

		bl IsOutOfDate() const
		{
			return _is_out_of_date;
		}

		mem::sptr<CommandStaging> BeginRenderCommandStaging()
		{
			mem::sptr<CommandStaging> command_staging;

			bl has_visibles = !_visibles.get_access()->empty() || true;
			if (has_visibles)
			{ // TODO: cleanup this
				VulkanRenderTarget& render_target = (VulkanRenderTarget&)*GetRenderTarget();
				VkExtent2D framebuffer_extent = render_target.GetFramebufferExtent();

				if (framebuffer_extent.width != 0 && framebuffer_extent.height != 0 &&
					!render_target.GetWindow()->IsMinimized())
				{
					VulkanRenderContext& render_context = (VulkanRenderContext&)*GetRenderContext();
					VulkanRenderDevice& render_device = (VulkanRenderDevice&)*GetRenderDevice();
					VkResult result = render_context.AcquireImage();
					if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
					{
						render_context.MarkAcquiredImageForUse();

						mem::sptr<VulkanCommandBuffer> command_buffer =
							render_context.GetCurrentFrame().commandBuffer;

						command_staging =
							mem::create_sptr<CommandStaging>(GetServices()->GetAllocator(), command_buffer);
						render_device.BeginCommandBuffer(command_buffer, _command_buffer_begin_info);
						NP_ENGINE_ASSERT(command_staging->IsValid(), "command_staging must be valid here");
					}
					else if (result == VK_ERROR_OUT_OF_DATE_KHR)
					{
						AdjustForOutOfDate();
					}
				}
			}

			return command_staging;
		}

		void EndRenderCommandStaging(mem::sptr<CommandStaging> command_staging)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)*GetRenderDevice();
			render_device.EndCommandBuffer(command_staging->GetCommandBuffer());
		}

		void BeginRenderPass(mem::sptr<CommandStaging> command_staging) override
		{
			VulkanRenderContext& vulkan_render_context = (VulkanRenderContext&)(*GetRenderContext());
			VulkanFramebuffers& vulkan_framebuffers = (VulkanFramebuffers&)(*GetRenderPipeline()->GetProperties().framebuffers);
			VulkanRenderPass& vulkan_render_pass = (VulkanRenderPass&)(*vulkan_framebuffers.GetRenderPass());

			_render_pass_begin_info.framebuffer = vulkan_framebuffers[vulkan_render_context.GetNextFrame().index];
			vulkan_render_pass.Begin(_render_pass_begin_info, command_staging);
		}

		void EndRenderPass(mem::sptr<CommandStaging> command_staging) override
		{
			mem::sptr<RenderPass> render_pass = GetRenderPipeline()->GetProperties().framebuffers->GetRenderPass();
			VulkanRenderPass& vulkan_render_pass = (VulkanRenderPass&)(*render_pass);
			vulkan_render_pass.End(command_staging);
		}

		void Render() override
		{
			NP_ENGINE_PROFILE_SCOPE("vulkan scene draw");

			// Scene::Render();
			_on_render_delegate.ConstructData<Scene*>(this);
			_on_render_delegate();
			mem::sptr<CommandStaging> command_staging = BeginRenderCommandStaging();

			if (command_staging && command_staging->IsValid())
			{
				VulkanRenderPipeline& render_pipeline = (VulkanRenderPipeline&)(*_properties.renderPipeline);

				UpdateCamera();
				PipelineMetaValues pipeline_meta_values = render_pipeline.GetMetaValues();
				pipeline_meta_values.View = _properties.camera.view;
				pipeline_meta_values.Projection = _properties.camera.projection;
				render_pipeline.SetMetaValues(pipeline_meta_values);

				BeginRenderPass(command_staging);

				render_pipeline.BindPipeline(command_staging);
				render_pipeline.PrepareToBindDescriptorSets(command_staging);
				{
					uid::UidSystem& uid_system = GetServices()->GetUidSystem();
					auto visibles = _visibles.get_access();
					for (auto it = visibles->begin(); it != visibles->end(); it++)
					{
						uid::Uid id = it->first;
						if (uid_system.Has(id))
						{
							mem::sptr<VisibleObject> visible = it->second;
							if (visible && _properties.camera.Contains(*visible))
							{
								mem::sptr<Resource> resource = GetRenderDevice()->GetResource(id);
								if (resource)
								{
									switch (resource->GetType())
									{
									case ResourceType::RenderableModel:
										((VulkanRenderableModel&)*resource).Stage(command_staging, _properties.renderPipeline);
										break;

									default:
										break;
									}
								}
							}
						}
						else
						{
							it->second.reset();
						}
					}
				}
				render_pipeline.BindDescriptorSets(command_staging);

				EndRenderPass(command_staging);
				command_staging->DigestCommands();

				EndRenderCommandStaging(command_staging);
				RenderCommandStaging(command_staging);
			}
		}

		void RenderCommandStaging(mem::sptr<CommandStaging> command_staging)
		{
			VulkanRenderContext& vulkan_render_context = (VulkanRenderContext&)(*GetRenderContext());
			VulkanRenderDevice& vulkan_render_device = (VulkanRenderDevice&)(*GetRenderDevice());

			NP_ENGINE_PROFILE_SCOPE("vulkan renderer draw frame");
			VulkanRenderFrame& current_frame = vulkan_render_context.GetCurrentFrame();
			VulkanRenderFrame& next_frame = vulkan_render_context.GetNextFrame();
			ui32 image_index = next_frame.index; // TODO: cleanup this area
			con::vector<VkCommandBuffer> buffers = {*current_frame.commandBuffer};
			con::vector<VkSemaphore> wait_semaphores{current_frame.imageSemaphore};
			con::vector<VkPipelineStageFlags> wait_stages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			con::vector<VkSemaphore> signal_semaphores{current_frame.renderSemaphore};
			con::vector<VkSwapchainKHR> swapchains{vulkan_render_context};

			VkSubmitInfo submit_info{};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.waitSemaphoreCount = wait_semaphores.size();
			submit_info.pWaitSemaphores = wait_semaphores.data();
			submit_info.pWaitDstStageMask = wait_stages.data();
			submit_info.commandBufferCount = (ui32)buffers.size();
			submit_info.pCommandBuffers = buffers.data();
			submit_info.signalSemaphoreCount = (ui32)signal_semaphores.size();
			submit_info.pSignalSemaphores = signal_semaphores.data();

			current_frame.fence.Reset();

			if (vulkan_render_device.GetGraphicsQueue()->Submit({submit_info}, current_frame.fence) != VK_SUCCESS)
				NP_ENGINE_ASSERT(false, "failed to submit draw command buffer!");

			VkPresentInfoKHR present_info{};
			present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present_info.waitSemaphoreCount = signal_semaphores.size();
			present_info.pWaitSemaphores = signal_semaphores.data();
			present_info.swapchainCount = swapchains.size();
			present_info.pSwapchains = swapchains.data();
			present_info.pImageIndices = &image_index;

			VkResult present_result = vkQueuePresentKHR(*vulkan_render_device.GetPresentQueue(), &present_info);

			if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR || IsOutOfDate())
				AdjustForOutOfDate();
			else if (present_result != VK_SUCCESS)
				NP_ENGINE_ASSERT(false, "vkQueuePresentKHR error");

			vkQueueWaitIdle(*vulkan_render_device.GetPresentQueue());
			vulkan_render_context.IncFrame();
		}

		virtual void SetCamera(Camera& camera) override
		{
			_properties.camera = camera;
		}

		virtual void AdjustForWindowResize(win::Window& window)
		{
			NP_ENGINE_LOG_INFO("scene adjusting for window resize?");
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_SCENE_HPP */