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

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanBuffer.hpp"
#include "VulkanFrame.hpp"
#include "VulkanDevice.hpp"
#include "VulkanTexture.hpp"
#include "VulkanCommands.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanDescriptorSets.hpp"
#include "VulkanSampler.hpp"
#include "VulkanFence.hpp"
#include "VulkanTimelineSemaphore.hpp"
#include "VulkanBinarySemaphore.hpp"

namespace np::gfx::rhi
{
	class VulkanRenderableModel : public RenderableModel
	{
	private:
		bl _is_out_of_date;
		VulkanBuffer* _vertex_buffer;
		VulkanBuffer* _index_buffer;
		VulkanTexture* _texture;
		VulkanSampler* _sampler;

		VulkanBuffer* _texture_staging;
		VulkanBuffer* _vertex_staging;
		VulkanBuffer* _index_staging;

		VkDescriptorImageInfo _descriptor_info;
		VkWriteDescriptorSet _descriptor_writer;

		VkBuffer _vk_vertex_buffer;
		con::vector<VkDeviceSize> _vertex_offsets;
		VulkanCommandPushConstants* _push_constants;
		VulkanCommandBindVertexBuffers* _bind_vertex_buffers;
		VulkanCommandBindIndexBuffer* _bind_index_buffer;
		VulkanCommandDrawIndexed* _draw_indexed;

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

		VulkanBuffer* CreateBuffer(VulkanDevice& device, VkDeviceSize size, VkBufferUsageFlags buffer_usage_flags,
								   VkMemoryPropertyFlags memory_property_flags)
		{
			VkBufferCreateInfo info = VulkanBuffer::CreateInfo();
			info.size = size;
			info.usage = buffer_usage_flags;

			return mem::Create<VulkanBuffer>(_services.GetAllocator(), device, info, memory_property_flags);
		}

		VulkanTexture* CreateTexture(VulkanDevice& device, VkImageCreateInfo& image_create_info,
									 VkMemoryPropertyFlags image_memory_property_flags,
									 VkImageViewCreateInfo& image_view_create_info, bl hot_reloadable)
		{
			return mem::Create<VulkanTexture>(_services.GetAllocator(), device, image_create_info, image_memory_property_flags,
											  image_view_create_info, hot_reloadable);
		}

		void BringUpToDateWithTimelineSemaphores(Pipeline& pipeline)
		{
			NP_ENGINE_PROFILE_FUNCTION();

			VulkanPipeline& vulkan_pipeline = (VulkanPipeline&)pipeline;
			VulkanDevice& vulkan_device = vulkan_pipeline.GetDevice();
			VkDeviceSize vertex_buffer_data_size = sizeof(VulkanVertex) * _model.GetVertices().size();
			VkDeviceSize index_buffer_data_size = sizeof(ui32) * _model.GetIndices().size();

			// create texture
			if (!_texture || _texture->IsHotReloadable() != _model.GetTexture().IsHotReloadable() ||
				_texture->GetWidth() != _model.GetTexture().GetWidth() ||
				_texture->GetHeight() != _model.GetTexture().GetHeight())
			{
				if (_texture)
					mem::Destroy<VulkanTexture>(_services.GetAllocator(), _texture);

				VkImageCreateInfo texture_image_create_info = VulkanImage::CreateInfo();
				texture_image_create_info.extent.width = _model.GetTexture().GetWidth();
				texture_image_create_info.extent.height = _model.GetTexture().GetHeight();
				texture_image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				VkMemoryPropertyFlags texture_memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				VkImageViewCreateInfo texture_image_view_create_info = VulkanImageView::CreateInfo();

				_texture = CreateTexture(vulkan_device, texture_image_create_info, texture_memory_property_flags,
										 texture_image_view_create_info, _model.GetTexture().IsHotReloadable());
			}

			// create vertex buffer
			if (!_vertex_buffer || _vertex_buffer->GetSize() != vertex_buffer_data_size)
			{
				if (_vertex_buffer)
					mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _vertex_buffer);

				_vertex_buffer = CreateBuffer(vulkan_device, vertex_buffer_data_size,
											  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
											  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			}

			// create index buffer
			if (!_index_buffer || _index_buffer->GetSize() != index_buffer_data_size)
			{
				if (_index_buffer)
					mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _index_buffer);

				_index_buffer = CreateBuffer(vulkan_device, index_buffer_data_size,
											 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
											 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			}

			// create staging
			if (!_texture_staging || _texture_staging->GetSize() != _model.GetTexture().Size())
			{
				if (_texture_staging)
					mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _texture_staging);

				_texture_staging = CreateBuffer(vulkan_device, _model.GetTexture().Size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
												VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			if (!_vertex_staging || _vertex_staging->GetSize() != vertex_buffer_data_size)
			{
				if (_vertex_staging)
					mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _vertex_staging);

				_vertex_staging = CreateBuffer(vulkan_device, vertex_buffer_data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
											   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			if (!_index_staging || _index_staging->GetSize() != index_buffer_data_size)
			{
				if (_index_staging)
					mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _index_staging);

				_index_staging = CreateBuffer(vulkan_device, index_buffer_data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
											  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			con::vector<VulkanCommandBuffer> command_buffers;

			// TODO: I think we can probably squeeze some more performance out of which stage our submits wait for
			con::vector<VkPipelineStageFlags> wait_dst_flags{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

			ui64 timeline_initial_value = 0;
			VulkanTimelineSemaphore timeline(vulkan_device, timeline_initial_value);
			VkSemaphore timeline_semaphore = timeline;

			VkTimelineSemaphoreSubmitInfo timeline_submit_info_template =
				VulkanTimelineSemaphore::CreateTimelineSemaphoreSubmitInfo();
			timeline_submit_info_template.waitSemaphoreValueCount = 1;
			timeline_submit_info_template.signalSemaphoreValueCount = 1;

			VkSubmitInfo submit_info_template = VulkanQueue::CreateSubmitInfo();
			submit_info_template.pWaitDstStageMask = wait_dst_flags.data();
			submit_info_template.waitSemaphoreCount = 1;
			submit_info_template.pWaitSemaphores = &timeline_semaphore;
			submit_info_template.signalSemaphoreCount = 1;
			submit_info_template.pSignalSemaphores = &timeline_semaphore;

			// create texture submit infos
			ui64 texture_transition_to_dst_value = timeline_initial_value;
			ui64 texture_assign_image_value = texture_transition_to_dst_value + 1;
			ui64 texture_transition_to_shader_value = texture_assign_image_value + 1;
			ui64 texture_complete_value = texture_transition_to_shader_value + 1;

			VkTimelineSemaphoreSubmitInfo trasition_to_dst_timeline_submit = timeline_submit_info_template;
			trasition_to_dst_timeline_submit.pWaitSemaphoreValues = &texture_transition_to_dst_value;
			trasition_to_dst_timeline_submit.pSignalSemaphoreValues = &texture_assign_image_value;

			VkTimelineSemaphoreSubmitInfo assign_image_timeline_submit = timeline_submit_info_template;
			assign_image_timeline_submit.pWaitSemaphoreValues = &texture_assign_image_value;
			assign_image_timeline_submit.pSignalSemaphoreValues = &texture_transition_to_shader_value;

			VkTimelineSemaphoreSubmitInfo transition_to_timeline_submit = timeline_submit_info_template;
			transition_to_timeline_submit.pWaitSemaphoreValues = &texture_transition_to_shader_value;
			transition_to_timeline_submit.pSignalSemaphoreValues = &texture_complete_value;

			VkSubmitInfo transition_to_dst_submit = submit_info_template;
			transition_to_dst_submit.pNext = &trasition_to_dst_timeline_submit;

			VkSubmitInfo assign_image_submit = submit_info_template;
			assign_image_submit.pNext = &assign_image_timeline_submit;

			VkSubmitInfo transition_to_shader_submit = submit_info_template;
			transition_to_shader_submit.pNext = &transition_to_timeline_submit;

			// copy image data to staging
			_texture_staging->AssignData(_model.GetTexture().Data());

			// copy staging to texture
			_texture->GetImage().AsyncTransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
													   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, transition_to_dst_submit,
													   command_buffers);

			VkBufferImageCopy buffer_image_copy = VulkanCommandCopyBufferToImage::CreateBufferImageCopy();
			buffer_image_copy.imageExtent = {_model.GetTexture().GetWidth(), _model.GetTexture().GetHeight(), 1};
			_texture->GetImage().AsyncAssign(*_texture_staging, buffer_image_copy, assign_image_submit, command_buffers);

			_texture->GetImage().AsyncTransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
													   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transition_to_shader_submit,
													   command_buffers);

			// create vertex submit infos
			ui64 vertex_buffer_copy_value = texture_complete_value;
			ui64 vertex_complete_value = vertex_buffer_copy_value + 1;

			VkTimelineSemaphoreSubmitInfo vertex_buffer_copy_timline_submit = timeline_submit_info_template;
			vertex_buffer_copy_timline_submit.pWaitSemaphoreValues = &vertex_buffer_copy_value;
			vertex_buffer_copy_timline_submit.pSignalSemaphoreValues = &vertex_complete_value;

			VkSubmitInfo vertex_buffer_copy_submit = submit_info_template;
			vertex_buffer_copy_submit.pNext = &vertex_buffer_copy_timline_submit;

			// copy vertex data to staging
			_vertex_staging->AssignData(_model.GetVertices().data());
			_vertex_staging->AsyncCopyTo(*_vertex_buffer, vertex_buffer_copy_submit, command_buffers);

			// create index submit infos
			ui64 index_buffer_copy_value = vertex_complete_value;
			ui64 index_complete_value = index_buffer_copy_value + 1;

			VkTimelineSemaphoreSubmitInfo index_buffer_copy_timeline_submit = timeline_submit_info_template;
			index_buffer_copy_timeline_submit.pWaitSemaphoreValues = &index_buffer_copy_value;
			index_buffer_copy_timeline_submit.pSignalSemaphoreValues = &index_complete_value;

			VkSubmitInfo index_buffer_copy_submit = submit_info_template;
			index_buffer_copy_submit.pNext = &index_buffer_copy_timeline_submit;

			// copy index data to staging
			_index_staging->AssignData(_model.GetIndices().data());
			_index_staging->AsyncCopyTo(*_index_buffer, index_buffer_copy_submit, command_buffers);

			// create commands
			_vk_vertex_buffer = *_vertex_buffer;

			if (!_push_constants)
			{
				_push_constants = mem::Create<VulkanCommandPushConstants>(
					_services.GetAllocator(), vulkan_pipeline.GetPipelineLayout(),
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(RenderableMetaValues), &_meta_values);
			}

			if (!_bind_vertex_buffers)
			{
				_bind_vertex_buffers = mem::Create<VulkanCommandBindVertexBuffers>(_services.GetAllocator(), 0, 1,
																				   &_vk_vertex_buffer, _vertex_offsets.data());
			}
			else
			{
				_bind_vertex_buffers->Buffers = &_vk_vertex_buffer;
			}

			if (!_bind_index_buffer)
			{
				_bind_index_buffer = mem::Create<VulkanCommandBindIndexBuffer>(_services.GetAllocator(), *_index_buffer, 0,
																			   VK_INDEX_TYPE_UINT32);
			}
			else
			{
				_bind_index_buffer->Buffer = *_index_buffer;
			}

			if (!_draw_indexed)
			{
				_draw_indexed = mem::Create<VulkanCommandDrawIndexed>(_services.GetAllocator(),
																	  (ui32)_model.GetIndices().size(), 1, 0, 0, 0);
			}
			else
			{
				_draw_indexed->IndexCount = (ui32)_model.GetIndices().size();
			}

			timeline.Wait(texture_complete_value);
			timeline.Wait(vertex_complete_value);
			timeline.Wait(index_complete_value);

			vulkan_device.FreeCommandBuffers(command_buffers);
			SetOutOfDate(false);
		}

		void BringUpToDateWithBinarySemahphores(Pipeline& pipeline)
		{
			NP_ENGINE_PROFILE_FUNCTION();

			VulkanPipeline& vulkan_pipeline = (VulkanPipeline&)pipeline;
			VulkanDevice& vulkan_device = vulkan_pipeline.GetDevice();
			VkDeviceSize vertex_buffer_data_size = sizeof(VulkanVertex) * _model.GetVertices().size();
			VkDeviceSize index_buffer_data_size = sizeof(ui32) * _model.GetIndices().size();

			// create texture
			if (!_texture)
			{
				VkImageCreateInfo texture_image_create_info = VulkanImage::CreateInfo();
				texture_image_create_info.extent.width = _model.GetTexture().GetWidth();
				texture_image_create_info.extent.height = _model.GetTexture().GetHeight();
				texture_image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				VkMemoryPropertyFlags texture_memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				VkImageViewCreateInfo texture_image_view_create_info = VulkanImageView::CreateInfo();
				_texture = CreateTexture(vulkan_device, texture_image_create_info, texture_memory_property_flags,
										 texture_image_view_create_info, _model.GetTexture().IsHotReloadable());
			}

			// create vertex buffer
			if (!_vertex_buffer || _vertex_buffer->GetSize() != vertex_buffer_data_size)
			{
				if (_vertex_buffer)
					mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _vertex_buffer);

				_vertex_buffer = CreateBuffer(vulkan_device, vertex_buffer_data_size,
											  VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
											  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			}

			// create index buffer
			if (!_index_buffer || _index_buffer->GetSize() != index_buffer_data_size)
			{
				_index_buffer = CreateBuffer(vulkan_device, index_buffer_data_size,
											 VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
											 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			}

			// create staging
			if (!_texture_staging || _texture_staging->GetSize() != _model.GetTexture().Size())
			{
				if (_texture_staging)
					mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _texture_staging);

				_texture_staging = CreateBuffer(vulkan_device, _model.GetTexture().Size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
												VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			if (!_vertex_staging || _vertex_staging->GetSize() != vertex_buffer_data_size)
			{
				if (_vertex_staging)
					mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _vertex_staging);

				_vertex_staging = CreateBuffer(vulkan_device, vertex_buffer_data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
											   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			if (!_index_staging || _index_staging->GetSize() != index_buffer_data_size)
			{
				if (_index_staging)
					mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _index_staging);

				_index_staging = CreateBuffer(vulkan_device, index_buffer_data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
											  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			con::vector<VulkanCommandBuffer> command_buffers;

			// TODO: I think we can probably squeeze some more performance out of which stage our submits wait for
			con::vector<VkPipelineStageFlags> wait_dst_flags{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

			// create texture submit infos
			VulkanBinarySemaphore transition_to_dst_semaphore(vulkan_device);
			VkSemaphore transition_to_dst_complete = transition_to_dst_semaphore;

			VulkanBinarySemaphore assign_image_semaphore(vulkan_device);
			VkSemaphore assign_image_complete = assign_image_semaphore;

			VulkanBinarySemaphore transition_to_shader_semaphore(vulkan_device);
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

			VulkanFence texture_complete_fence(vulkan_device);

			// copy image data to staging
			_texture_staging->AssignData(_model.GetTexture().Data());

			// copy staging to texture
			_texture->GetImage().AsyncTransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
													   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, transition_to_dst_submit,
													   command_buffers);

			VkBufferImageCopy buffer_image_copy = VulkanCommandCopyBufferToImage::CreateBufferImageCopy();
			buffer_image_copy.imageExtent = {_model.GetTexture().GetWidth(), _model.GetTexture().GetHeight(), 1};
			_texture->GetImage().AsyncAssign(*_texture_staging, buffer_image_copy, assign_image_submit, command_buffers);

			_texture->GetImage().AsyncTransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
													   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, transition_to_shader_submit,
													   command_buffers, texture_complete_fence);

			// create vertex submit infos
			VulkanBinarySemaphore vertex_buffer_copy_semaphore(vulkan_device);
			VkSemaphore vertex_buffer_copy_complete = vertex_buffer_copy_semaphore;

			VkSubmitInfo vertex_buffer_copy_submit = VulkanQueue::CreateSubmitInfo();
			vertex_buffer_copy_submit.pWaitDstStageMask = wait_dst_flags.data();
			vertex_buffer_copy_submit.waitSemaphoreCount = transition_to_shader_submit.signalSemaphoreCount;
			vertex_buffer_copy_submit.pWaitSemaphores = transition_to_shader_submit.pSignalSemaphores;
			vertex_buffer_copy_submit.signalSemaphoreCount = 1;
			vertex_buffer_copy_submit.pSignalSemaphores = &vertex_buffer_copy_complete;

			VulkanFence vertex_complete_fence(vulkan_device);

			// copy vertex data to staging
			_vertex_staging->AssignData(_model.GetVertices().data());
			_vertex_staging->AsyncCopyTo(*_vertex_buffer, vertex_buffer_copy_submit, command_buffers, vertex_complete_fence);

			// create index submit infos
			VulkanBinarySemaphore index_buffer_copy_semaphore(vulkan_device);
			VkSemaphore index_buffer_copy_complete = index_buffer_copy_semaphore;

			VkSubmitInfo index_buffer_copy_submit = VulkanQueue::CreateSubmitInfo();
			index_buffer_copy_submit.pWaitDstStageMask = wait_dst_flags.data();
			index_buffer_copy_submit.waitSemaphoreCount = vertex_buffer_copy_submit.signalSemaphoreCount;
			index_buffer_copy_submit.pWaitSemaphores = vertex_buffer_copy_submit.pSignalSemaphores;
			index_buffer_copy_submit.signalSemaphoreCount = 1;
			index_buffer_copy_submit.pSignalSemaphores = &index_buffer_copy_complete;

			VulkanFence index_complete_fence(vulkan_device);

			// copy index data to staging
			_index_staging->AssignData(_model.GetIndices().data());
			_index_staging->AsyncCopyTo(*_index_buffer, index_buffer_copy_submit, command_buffers, index_complete_fence);

			// create commands
			_vk_vertex_buffer = *_vertex_buffer;

			if (!_push_constants)
			{
				_push_constants = mem::Create<VulkanCommandPushConstants>(
					_services.GetAllocator(), vulkan_pipeline.GetPipelineLayout(),
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(RenderableMetaValues), &_meta_values);
			}

			if (!_bind_vertex_buffers)
			{
				_bind_vertex_buffers = mem::Create<VulkanCommandBindVertexBuffers>(_services.GetAllocator(), 0, 1,
																				   &_vk_vertex_buffer, _vertex_offsets.data());
			}
			else
			{
				_bind_vertex_buffers->Buffers = &_vk_vertex_buffer;
			}

			if (!_bind_index_buffer)
			{
				_bind_index_buffer = mem::Create<VulkanCommandBindIndexBuffer>(_services.GetAllocator(), *_index_buffer, 0,
																			   VK_INDEX_TYPE_UINT32);
			}
			else
			{
				_bind_index_buffer->Buffer = *_index_buffer;
			}

			if (!_draw_indexed)
			{
				_draw_indexed = mem::Create<VulkanCommandDrawIndexed>(_services.GetAllocator(),
																	  (ui32)_model.GetIndices().size(), 1, 0, 0, 0);
			}
			else
			{
				_draw_indexed->IndexCount = (ui32)_model.GetIndices().size();
			}

			texture_complete_fence.Wait();
			vertex_complete_fence.Wait();
			index_complete_fence.Wait();

			vulkan_device.FreeCommandBuffers(command_buffers);
			SetOutOfDate(false);
		}

		void Dispose()
		{
			if (_texture_staging)
			{
				mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _texture_staging);
				_texture_staging = nullptr;
			}

			if (_vertex_staging)
			{
				mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _vertex_staging);
				_vertex_staging = nullptr;
			}

			if (_index_staging)
			{
				mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _index_staging);
				_index_staging = nullptr;
			}

			if (_texture)
			{
				mem::Destroy<VulkanTexture>(_services.GetAllocator(), _texture);
				_texture = nullptr;
			}

			if (_vertex_buffer)
			{
				mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _vertex_buffer);
				_vertex_buffer = nullptr;
			}

			if (_index_buffer)
			{
				mem::Destroy<VulkanBuffer>(_services.GetAllocator(), _index_buffer);
				_index_buffer = nullptr;
			}

			if (_push_constants)
			{
				mem::Destroy<VulkanCommandPushConstants>(_services.GetAllocator(), _push_constants);
				_push_constants = nullptr;
			}

			if (_bind_vertex_buffers)
			{
				mem::Destroy<VulkanCommandBindVertexBuffers>(_services.GetAllocator(), _bind_vertex_buffers);
				_bind_vertex_buffers = nullptr;
			}

			if (_bind_index_buffer)
			{
				mem::Destroy<VulkanCommandBindIndexBuffer>(_services.GetAllocator(), _bind_index_buffer);
				_bind_index_buffer = nullptr;
			}

			if (_draw_indexed)
			{
				mem::Destroy<VulkanCommandDrawIndexed>(_services.GetAllocator(), _draw_indexed);
				_draw_indexed = nullptr;
			}

			SetOutOfDate();
		}

	public:
		VulkanRenderableModel(srvc::Services& services, Model& model):
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

		void RenderToFrame(Frame& frame, Pipeline& pipeline) override
		{
			VulkanFrame& vulkan_frame = (VulkanFrame&)frame;
			VulkanPipeline& vulkan_pipeline = (VulkanPipeline&)pipeline;

			PrepareForPipeline(pipeline);
			_update_meta_values_on_frame.InvokeConnectedFunction();

			vulkan_frame.StageCommand(*_push_constants);
			vulkan_frame.StageCommand(*_bind_vertex_buffers);
			vulkan_frame.StageCommand(*_bind_index_buffer);
			vulkan_frame.StageCommand(*_draw_indexed);
		}

		void PrepareForPipeline(Pipeline& pipeline) override
		{
			SetOutOfDate();

			if (IsOutOfDate())
			{
				NP_ENGINE_PROFILE_SCOPE("preparing for pipeline");

				// TODO: I'm not too convinced that we need to support timeline semaphores... we should profile it
				// TODO: this method needs to be updated for hot-reloadable textures? I'm not so sure...

#if NP_ENGINE_PLATFORM_SUPPORTS_VULKAN_TIMELINE_SEMAPHORES
				BringUpToDateWithTimelineSemaphores(pipeline);
#else
				BringUpToDateWithBinarySemahphores(pipeline);
#endif
			}

			VulkanPipeline& vulkan_pipeline = (VulkanPipeline&)pipeline;
			_push_constants->PipelineLayout = vulkan_pipeline.GetPipelineLayout();

			if (!_sampler)
				SetSampler(vulkan_pipeline.GetDefaultSampler());

			_descriptor_info.imageView = _texture ? (VkImageView)_texture->GetImageView() : nullptr;
			_descriptor_info.sampler = *_sampler;

			vulkan_pipeline.GetDescriptorSets().SubmitWriter(_descriptor_writer);
		}

		void DisposeForPipeline(Pipeline& pipeline) override
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

		void SetSampler(VulkanSampler& sampler)
		{
			_sampler = mem::AddressOf(sampler);
			_descriptor_info.sampler = *_sampler;
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_RENDERABLE_MODEL_HPP */