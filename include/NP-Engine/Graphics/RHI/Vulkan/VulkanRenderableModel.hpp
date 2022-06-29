//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDERABLE_MODEL_HPP
#define NP_ENGINE_VULKAN_RENDERABLE_MODEL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

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

namespace np::graphics::rhi
{
	class VulkanRenderableModel : public RenderableModel
	{
	private:
		bl _is_out_of_date;
		VulkanBuffer* _vertex_buffer;
		VulkanBuffer* _index_buffer;
		VulkanTexture* _texture;
		VulkanSampler* _sampler;

		VkDescriptorImageInfo _descriptor_info;
		VkWriteDescriptorSet _descriptor_writer;

		VkBuffer _vk_vertex_buffer;
		container::vector<VkDeviceSize> _vertex_offsets;
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

			return memory::Create<VulkanBuffer>(memory::DefaultTraitAllocator, device, info, memory_property_flags);
		}

		VulkanTexture* CreateTexture(VulkanDevice& device, VkImageCreateInfo& image_create_info,
									 VkMemoryPropertyFlags image_memory_property_flags,
									 VkImageViewCreateInfo& image_view_create_info)
		{
			return memory::Create<VulkanTexture>(memory::DefaultTraitAllocator, device, image_create_info,
												 image_memory_property_flags, image_view_create_info);
		}

		void Destruct() override
		{
			Dispose();
		}

		void Dispose()
		{
			if (_texture)
			{
				memory::Destroy<VulkanTexture>(memory::DefaultTraitAllocator, _texture);
				_texture = nullptr;
			}

			if (_vertex_buffer)
			{
				memory::Destroy<VulkanBuffer>(memory::DefaultTraitAllocator, _vertex_buffer);
				_vertex_buffer = nullptr;
			}

			if (_index_buffer)
			{
				memory::Destroy<VulkanBuffer>(memory::DefaultTraitAllocator, _index_buffer);
				_index_buffer = nullptr;
			}

			if (_push_constants)
			{
				memory::Destroy<VulkanCommandPushConstants>(memory::DefaultTraitAllocator, _push_constants);
				_push_constants = nullptr;
			}

			if (_bind_vertex_buffers)
			{
				memory::Destroy<VulkanCommandBindVertexBuffers>(memory::DefaultTraitAllocator, _bind_vertex_buffers);
				_bind_vertex_buffers = nullptr;
			}

			if (_bind_index_buffer)
			{
				memory::Destroy<VulkanCommandBindIndexBuffer>(memory::DefaultTraitAllocator, _bind_index_buffer);
				_bind_index_buffer = nullptr;
			}

			if (_draw_indexed)
			{
				memory::Destroy<VulkanCommandDrawIndexed>(memory::DefaultTraitAllocator, _draw_indexed);
				_draw_indexed = nullptr;
			}
		}

	public:
		VulkanRenderableModel(Model& model):
			RenderableModel(model),
			_is_out_of_date(true),
			_vertex_buffer(nullptr),
			_index_buffer(nullptr),
			_texture(nullptr),
			_sampler(nullptr),
			_descriptor_info(CreateDescriptorImageInfo()),
			_descriptor_writer(CreateDescriptorWriter()),
			_vk_vertex_buffer(nullptr),
			_vertex_offsets({0}),
			_push_constants(nullptr),
			_bind_vertex_buffers(nullptr),
			_bind_index_buffer(nullptr),
			_draw_indexed(nullptr)
		{}

		// TODO: move constructor??

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
			VulkanPipeline& vulkan_pipeline = (VulkanPipeline&)pipeline;
			VulkanDevice& vulkan_device = vulkan_pipeline.GetDevice();

			if (IsOutOfDate())
			{
				DisposeForPipeline(pipeline);
				container::vector<VulkanCommandBuffer> command_buffers;

				// TODO: FIX BROKE BUILDS: macos and linux cannot use timeline semaphores - maybe use khr extension of it??

				// TODO: I think we can probably squeeze some more performance out of which stage our submits wait for
				container::vector<VkPipelineStageFlags> wait_dst_flags{VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT};

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

				// create texture
				VkImageCreateInfo texture_image_create_info = VulkanImage::CreateInfo();
				texture_image_create_info.extent.width = _model.GetTextureImage().GetWidth();
				texture_image_create_info.extent.height = _model.GetTextureImage().GetHeight();
				texture_image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				VkMemoryPropertyFlags texture_memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				VkImageViewCreateInfo texture_image_view_create_info = VulkanImageView::CreateInfo();
				_texture = CreateTexture(vulkan_device, texture_image_create_info, texture_memory_property_flags,
										 texture_image_view_create_info);

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

				// create staging buffer
				VulkanBuffer* texture_staging =
					CreateBuffer(vulkan_device, _model.GetTextureImage().Size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				// copy image data to staging
				texture_staging->AssignData(_model.GetTextureImage().Data());

				// copy staging to texture
				_texture->GetImage().AsyncTransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
														   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, transition_to_dst_submit,
														   command_buffers);

				VkBufferImageCopy buffer_image_copy = VulkanCommandCopyBufferToImage::CreateBufferImageCopy();
				buffer_image_copy.imageExtent = {_model.GetTextureImage().GetWidth(), _model.GetTextureImage().GetHeight(), 1};
				_texture->GetImage().AsyncAssign(*texture_staging, buffer_image_copy, assign_image_submit, command_buffers);

				_texture->GetImage().AsyncTransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
														   VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
														   transition_to_shader_submit, command_buffers);

				// create vertex submit infos
				ui64 vertex_buffer_copy_value = texture_complete_value;
				ui64 vertex_complete_value = vertex_buffer_copy_value + 1;

				VkTimelineSemaphoreSubmitInfo vertex_buffer_copy_timline_submit = timeline_submit_info_template;
				vertex_buffer_copy_timline_submit.pWaitSemaphoreValues = &vertex_buffer_copy_value;
				vertex_buffer_copy_timline_submit.pSignalSemaphoreValues = &vertex_complete_value;

				VkSubmitInfo vertex_buffer_copy_submit = submit_info_template;
				vertex_buffer_copy_submit.pNext = &vertex_buffer_copy_timline_submit;

				// create vertex buffer
				VkDeviceSize data_size = sizeof(VulkanVertex) * _model.GetVertices().size();

				VulkanBuffer* vertex_staging =
					CreateBuffer(vulkan_device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				_vertex_buffer =
					CreateBuffer(vulkan_device, data_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

				vertex_staging->AssignData(_model.GetVertices().data());
				vertex_staging->AsyncCopyTo(*_vertex_buffer, vertex_buffer_copy_submit, command_buffers);

				// create index submit infos
				ui64 index_buffer_copy_value = vertex_complete_value;
				ui64 index_complete_value = index_buffer_copy_value + 1;

				VkTimelineSemaphoreSubmitInfo index_buffer_copy_timeline_submit = timeline_submit_info_template;
				index_buffer_copy_timeline_submit.pWaitSemaphoreValues = &index_buffer_copy_value;
				index_buffer_copy_timeline_submit.pSignalSemaphoreValues = &index_complete_value;

				VkSubmitInfo index_buffer_copy_submit = submit_info_template;
				index_buffer_copy_submit.pNext = &index_buffer_copy_timeline_submit;

				// create index buffer
				data_size = sizeof(ui32) * _model.GetIndices().size();

				VulkanBuffer* index_staging =
					CreateBuffer(vulkan_device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				_index_buffer =
					CreateBuffer(vulkan_device, data_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

				index_staging->AssignData(_model.GetIndices().data());
				index_staging->AsyncCopyTo(*_index_buffer, index_buffer_copy_submit, command_buffers);

				// create commands
				_vk_vertex_buffer = *_vertex_buffer;

				_push_constants = memory::Create<VulkanCommandPushConstants>(
					memory::DefaultTraitAllocator, vulkan_pipeline.GetPipelineLayout(),
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(RenderableMetaValues), &_meta_values);

				_bind_vertex_buffers = memory::Create<VulkanCommandBindVertexBuffers>(
					memory::DefaultTraitAllocator, 0, 1, &_vk_vertex_buffer, _vertex_offsets.data());

				_bind_index_buffer = memory::Create<VulkanCommandBindIndexBuffer>(memory::DefaultTraitAllocator, *_index_buffer,
																				  0, VK_INDEX_TYPE_UINT32);

				_draw_indexed = memory::Create<VulkanCommandDrawIndexed>(memory::DefaultTraitAllocator,
																		 (ui32)_model.GetIndices().size(), 1, 0, 0, 0);

				timeline.Wait(texture_complete_value);
				memory::Destroy<VulkanBuffer>(memory::DefaultTraitAllocator, texture_staging);
				timeline.Wait(vertex_complete_value);
				memory::Destroy<VulkanBuffer>(memory::DefaultTraitAllocator, vertex_staging);
				timeline.Wait(index_complete_value);
				memory::Destroy<VulkanBuffer>(memory::DefaultTraitAllocator, index_staging);

				vulkan_device.FreeCommandBuffers(command_buffers);
				SetOutOfDate(false);
			}

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
			_sampler = memory::AddressOf(sampler);
			_descriptor_info.sampler = *_sampler;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_RENDERABLE_MODEL_HPP */