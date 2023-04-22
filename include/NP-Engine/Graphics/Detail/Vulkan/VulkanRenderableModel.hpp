//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDERABLE_MODEL_HPP
#define NP_ENGINE_VULKAN_RENDERABLE_MODEL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "NP-Engine/Graphics/Interface/Interface.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanLogicalDevice.hpp"
#include "VulkanTexture.hpp"
#include "VulkanCommands.hpp"
#include "VulkanDescriptorSets.hpp"
#include "VulkanSampler.hpp"
#include "VulkanFence.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanRenderPipeline.hpp"

namespace np::gfx::__detail
{
	class VulkanRenderableModel : public RenderableModel
	{
	private:
		bl _is_out_of_date;
		mem::sptr<VulkanBuffer> _vertex_buffer;
		mem::sptr<VulkanBuffer> _index_buffer;
		mem::sptr<VulkanTexture> _texture;
		mem::sptr<VulkanSampler> _sampler;

		mem::sptr<VulkanBuffer> _texture_staging;
		mem::sptr<VulkanBuffer> _vertex_staging;
		mem::sptr<VulkanBuffer> _index_staging;

		VkDescriptorImageInfo _descriptor_info;
		VkWriteDescriptorSet _descriptor_writer;

		VkBuffer _vk_vertex_buffer;
		con::vector<VkDeviceSize> _vertex_offsets;
		mem::sptr<VulkanCommandPushConstants> _push_constants;
		mem::sptr<VulkanCommandBindVertexBuffers> _bind_vertex_buffers;
		mem::sptr<VulkanCommandBindIndexBuffer> _bind_index_buffer;
		mem::sptr<VulkanCommandDrawIndexed> _draw_indexed;

		VkDescriptorImageInfo CreateDescriptorImageInfo()
		{
			VkDescriptorImageInfo info{};
			info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			return info;
		}

		VkWriteDescriptorSet CreateDescriptorWriter()
		{
			VkWriteDescriptorSet writer{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
			writer.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writer.pImageInfo = &_descriptor_info;
			writer.descriptorCount = 1;
			return writer;
		}

		mem::sptr<VulkanBuffer> CreateBuffer(mem::sptr<VulkanCommandPool> command_pool, VkDeviceSize size, VkBufferUsageFlags buffer_usage_flags,
								   VkMemoryPropertyFlags memory_property_flags)
		{
			VkBufferCreateInfo info = VulkanBuffer::CreateInfo();
			info.size = size;
			info.usage = buffer_usage_flags;

			return mem::create_sptr<VulkanBuffer>(_services->GetAllocator(), command_pool, info, memory_property_flags);
		}

		mem::sptr<VulkanTexture> CreateTexture(mem::sptr<VulkanCommandPool> command_pool, VkImageCreateInfo& image_create_info,
									 VkMemoryPropertyFlags image_memory_property_flags,
									 VkImageViewCreateInfo& image_view_create_info, bl hot_reloadable)
		{
			return mem::create_sptr<VulkanTexture>(_services->GetAllocator(), command_pool, image_create_info, image_memory_property_flags,
											  image_view_create_info, hot_reloadable);
		}

		void BringUpToDate(mem::sptr<Pipeline> pipeline)
		{
			NP_ENGINE_PROFILE_FUNCTION();

			VulkanRenderPipeline& vulkan_render_pipeline = (VulkanRenderPipeline&)(*pipeline);
			mem::sptr<Framebuffers> framebuffers = vulkan_render_pipeline.GetProperties().framebuffers;
			mem::sptr<RenderPass> render_pass = framebuffers->GetRenderPass();
			mem::sptr<RenderContext> render_context = render_pass->GetRenderContext();
			mem::sptr<RenderDevice> render_device = render_context->GetRenderDevice();
			VulkanRenderDevice& vulkan_render_device = (VulkanRenderDevice&)(*render_device);
			mem::sptr<VulkanCommandPool> vulkan_command_pool = vulkan_render_device.GetCommandPool();
			mem::sptr<VulkanLogicalDevice> vulkan_logical_device = vulkan_render_device.GetLogicalDevice();
			mem::sptr<VulkanQueue> vulkan_graphics_queue = vulkan_render_device.GetGraphicsQueue();

			VkDeviceSize vertex_buffer_data_size = sizeof(VulkanVertex) * _model->GetVertices().size();
			VkDeviceSize index_buffer_data_size = sizeof(ui32) * _model->GetIndices().size();

			// create texture
			if (!_texture)
			{
				VkImageCreateInfo texture_image_create_info = VulkanImage::CreateInfo();
				texture_image_create_info.extent.width = _model->GetTexture().GetWidth();
				texture_image_create_info.extent.height = _model->GetTexture().GetHeight();
				texture_image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				VkMemoryPropertyFlags texture_memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				VkImageViewCreateInfo texture_image_view_create_info = VulkanImageView::CreateInfo();
				_texture = CreateTexture(vulkan_command_pool, texture_image_create_info, texture_memory_property_flags,
										 texture_image_view_create_info, _model->GetTexture().IsHotReloadable());
			}

			// create vertex buffer
			if (!_vertex_buffer || _vertex_buffer->GetSize() != vertex_buffer_data_size)
			{
				_vertex_buffer = CreateBuffer(vulkan_command_pool, vertex_buffer_data_size,
											  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
											  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			}

			// create index buffer
			if (!_index_buffer || _index_buffer->GetSize() != index_buffer_data_size)
			{
				_index_buffer = CreateBuffer(vulkan_command_pool, index_buffer_data_size,
											 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
											 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			}

			// create staging
			if (!_texture_staging || _texture_staging->GetSize() != _model->GetTexture().Size())
			{
				_texture_staging = CreateBuffer(vulkan_command_pool, _model->GetTexture().Size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
												VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			if (!_vertex_staging || _vertex_staging->GetSize() != vertex_buffer_data_size)
			{
				_vertex_staging = CreateBuffer(vulkan_command_pool, vertex_buffer_data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
											   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			if (!_index_staging || _index_staging->GetSize() != index_buffer_data_size)
			{
				_index_staging = CreateBuffer(vulkan_command_pool, index_buffer_data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
											  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			con::vector<mem::sptr<VulkanCommandBuffer>> command_buffers;

			// TODO: I think we can probably squeeze some more performance out of which stage our submits wait for
			con::vector<VkPipelineStageFlags> wait_dst_flags{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

			// create texture submit infos
			VulkanSemaphore transition_to_dst_semaphore(vulkan_logical_device);
			VkSemaphore transition_to_dst_complete = transition_to_dst_semaphore;

			VulkanSemaphore assign_image_semaphore(vulkan_logical_device);
			VkSemaphore assign_image_complete = assign_image_semaphore;

			VulkanSemaphore transition_to_shader_semaphore(vulkan_logical_device);
			VkSemaphore transition_to_shader_complete = transition_to_shader_semaphore;

			VkSubmitInfo transition_to_dst_submit = VulkanQueue::CreateSubmitInfo();
			transition_to_dst_submit.pWaitDstStageMask = wait_dst_flags.data();
			transition_to_dst_submit.signalSemaphoreCount = 1;
			transition_to_dst_submit.pSignalSemaphores = &transition_to_dst_complete;

			VkSubmitInfo assign_image_submit = VulkanQueue::CreateSubmitInfo();
			assign_image_submit.pWaitDstStageMask = wait_dst_flags.data();
			assign_image_submit.waitSemaphoreCount = transition_to_dst_submit.signalSemaphoreCount;
			assign_image_submit.pWaitSemaphores = transition_to_dst_submit.pSignalSemaphores;
			assign_image_submit.signalSemaphoreCount = 1;
			assign_image_submit.pSignalSemaphores = &assign_image_complete;

			VkSubmitInfo transition_to_shader_submit = VulkanQueue::CreateSubmitInfo();
			transition_to_shader_submit.pWaitDstStageMask = wait_dst_flags.data();
			transition_to_shader_submit.waitSemaphoreCount = assign_image_submit.signalSemaphoreCount;
			transition_to_shader_submit.pWaitSemaphores = assign_image_submit.pSignalSemaphores;
			transition_to_shader_submit.signalSemaphoreCount = 1;
			transition_to_shader_submit.pSignalSemaphores = &transition_to_shader_complete;

			VulkanFence texture_complete_fence(vulkan_logical_device);

			// copy image data to staging
			_texture_staging->AssignData(_model->GetTexture().Data());

			// copy staging to texture
			_texture->GetImage().AsyncTransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
													   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, transition_to_dst_submit,
													   command_buffers, vulkan_graphics_queue);

			VkBufferImageCopy buffer_image_copy = VulkanCommandCopyBufferToImage::CreateBufferImageCopy();
			buffer_image_copy.imageExtent = {_model->GetTexture().GetWidth(), _model->GetTexture().GetHeight(), 1};
			_texture->GetImage().AsyncAssign(*_texture_staging, buffer_image_copy, assign_image_submit, command_buffers, vulkan_graphics_queue);

			_texture->GetImage().AsyncTransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
													   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transition_to_shader_submit,
													   command_buffers, vulkan_graphics_queue, texture_complete_fence);

			// create vertex submit infos
			VulkanSemaphore vertex_buffer_copy_semaphore(vulkan_logical_device);
			VkSemaphore vertex_buffer_copy_complete = vertex_buffer_copy_semaphore;

			VkSubmitInfo vertex_buffer_copy_submit = VulkanQueue::CreateSubmitInfo();
			vertex_buffer_copy_submit.pWaitDstStageMask = wait_dst_flags.data();
			vertex_buffer_copy_submit.waitSemaphoreCount = transition_to_shader_submit.signalSemaphoreCount;
			vertex_buffer_copy_submit.pWaitSemaphores = transition_to_shader_submit.pSignalSemaphores;
			vertex_buffer_copy_submit.signalSemaphoreCount = 1;
			vertex_buffer_copy_submit.pSignalSemaphores = &vertex_buffer_copy_complete;

			VulkanFence vertex_complete_fence(vulkan_logical_device);

			// copy vertex data to staging
			_vertex_staging->AssignData(_model->GetVertices().data());
			_vertex_staging->AsyncCopyTo(*_vertex_buffer, vertex_buffer_copy_submit, command_buffers, vulkan_graphics_queue, vertex_complete_fence);

			// create index submit infos
			VulkanSemaphore index_buffer_copy_semaphore(vulkan_logical_device);
			VkSemaphore index_buffer_copy_complete = index_buffer_copy_semaphore;

			VkSubmitInfo index_buffer_copy_submit = VulkanQueue::CreateSubmitInfo();
			index_buffer_copy_submit.pWaitDstStageMask = wait_dst_flags.data();
			index_buffer_copy_submit.waitSemaphoreCount = vertex_buffer_copy_submit.signalSemaphoreCount;
			index_buffer_copy_submit.pWaitSemaphores = vertex_buffer_copy_submit.pSignalSemaphores;
			index_buffer_copy_submit.signalSemaphoreCount = 1;
			index_buffer_copy_submit.pSignalSemaphores = &index_buffer_copy_complete;

			VulkanFence index_complete_fence(vulkan_logical_device);

			// copy index data to staging
			_index_staging->AssignData(_model->GetIndices().data());
			_index_staging->AsyncCopyTo(*_index_buffer, index_buffer_copy_submit, command_buffers, vulkan_graphics_queue, index_complete_fence);

			// create commands
			_vk_vertex_buffer = *_vertex_buffer;

			if (!_push_constants)
			{
				_push_constants = mem::create_sptr<VulkanCommandPushConstants>(
					_services->GetAllocator(), vulkan_render_pipeline.GetLayout(),
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(RenderableMetaValues), &_meta_values);
			}

			if (!_bind_vertex_buffers)
			{
				_bind_vertex_buffers = mem::create_sptr<VulkanCommandBindVertexBuffers>(_services->GetAllocator(), 0, 1,
																				   &_vk_vertex_buffer, _vertex_offsets.data());
			}
			else
			{
				_bind_vertex_buffers->Buffers = &_vk_vertex_buffer;
			}

			if (!_bind_index_buffer)
			{
				_bind_index_buffer = mem::create_sptr<VulkanCommandBindIndexBuffer>(_services->GetAllocator(), *_index_buffer, 0,
																			   VK_INDEX_TYPE_UINT32);
			}
			else
			{
				_bind_index_buffer->Buffer = *_index_buffer;
			}

			if (!_draw_indexed)
			{
				_draw_indexed = mem::create_sptr<VulkanCommandDrawIndexed>(_services->GetAllocator(),
																	  (ui32)_model->GetIndices().size(), 1, 0, 0, 0);
			}
			else
			{
				_draw_indexed->IndexCount = (ui32)_model->GetIndices().size();
			}

			texture_complete_fence.Wait();
			vertex_complete_fence.Wait();
			index_complete_fence.Wait();

			vulkan_command_pool->FreeCommandBuffers(command_buffers);
			SetOutOfDate(false);
		}

		void Dispose()
		{
			_texture_staging.reset();
			_vertex_staging.reset();
			_index_staging.reset();
			_texture.reset();
			_vertex_buffer.reset();
			_index_buffer.reset();
			_push_constants.reset();
			_bind_vertex_buffers.reset();
			_bind_index_buffer.reset();
			_draw_indexed.reset();

			SetOutOfDate();
		}

	public:
		VulkanRenderableModel(mem::sptr<srvc::Services> services, mem::sptr<Model> model):
			RenderableModel(services, model),
			_is_out_of_date(true),
			_vertex_buffer(nullptr),
			_index_buffer(nullptr),
			_texture(nullptr),
			_sampler(nullptr),
			_texture_staging(nullptr),
			_vertex_staging(nullptr),
			_index_staging(nullptr),
			_descriptor_info(CreateDescriptorImageInfo()),
			_descriptor_writer(CreateDescriptorWriter()),
			_vk_vertex_buffer(nullptr),
			_vertex_offsets({0}),
			_push_constants(nullptr),
			_bind_vertex_buffers(nullptr),
			_bind_index_buffer(nullptr),
			_draw_indexed(nullptr)
		{
			// TODO: feel like we can go ahead and get some resources here
		}

		// TODO: move constructor??

		~VulkanRenderableModel()
		{
			Dispose();
		}

		void Stage(mem::sptr<CommandStaging> staging, mem::sptr<Pipeline> pipeline) override
		{
			PrepareForPipeline(pipeline);
			//_update_meta_values_on_frame(); //TODO: ??

			staging->Stage(_push_constants);
			staging->Stage(_bind_vertex_buffers);
			staging->Stage(_bind_index_buffer);
			staging->Stage(_draw_indexed);
		}

		void PrepareForPipeline(mem::sptr<Pipeline> pipeline) override
		{
			SetOutOfDate();

			if (IsOutOfDate())
			{
				NP_ENGINE_PROFILE_SCOPE("preparing for pipeline");
				BringUpToDate(pipeline);
			}

			VulkanRenderPipeline& vulkan_render_pipeline = (VulkanRenderPipeline&)(*pipeline);
			_push_constants->PipelineLayout = vulkan_render_pipeline.GetLayout();

			if (!_sampler)
				SetSampler(vulkan_render_pipeline.GetDefaultSampler());

			_descriptor_info.imageView = _texture ? (VkImageView)_texture->GetImageView() : nullptr;

			vulkan_render_pipeline.GetDescriptorSets()->SubmitWriter(_descriptor_writer);
		}

		void DisposeForPipeline(mem::sptr<Pipeline> pipeline) override
		{
			Dispose();
		}

		bl IsOutOfDate() const override
		{
			return _is_out_of_date;
		}

		void SetOutOfDate(bl is_out_of_date = true) override
		{
			_is_out_of_date = is_out_of_date;
		}

		void SetSampler(mem::sptr<VulkanSampler> sampler)
		{
			_sampler = sampler;
			_descriptor_info.sampler = *_sampler;
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_RENDERABLE_MODEL_HPP */