//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDERABLE_MODEL_HPP
#define NP_ENGINE_VULKAN_RENDERABLE_MODEL_HPP

#include <iostream> //TODO: remove

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Time/Time.hpp"

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

namespace np::graphics::rhi
{
	class VulkanRenderableModel : public RenderableModel
	{
	private:
		bl _is_out_of_date;
		ObjectMetaValues
			_meta_values; // TODO: we could put this in RenderableModel so GameLayer can access it and update MetaValues there
		VulkanBuffer* _vertex_buffer;
		VulkanBuffer* _index_buffer;
		VulkanTexture* _texture;
		VulkanSampler* _sampler;

		VkDescriptorImageInfo _descriptor_info;
		VkWriteDescriptorSet _descriptor_writer;

		time::SteadyTimestamp _start_timestamp;

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
			_start_timestamp(time::SteadyClock::now()),
			_vk_vertex_buffer(nullptr),
			_vertex_offsets({0}),
			_push_constants(nullptr),
			_bind_vertex_buffers(nullptr),
			_bind_index_buffer(nullptr),
			_draw_indexed(nullptr)
		{}

		// TODO: move constructor??

		~VulkanRenderableModel()
		{
			::std::cout << "loud VulkanRenderableModel destructor!\n";

			Dispose();
		}

		void UpdateMetaValues()
		{
			// TODO: still not completely happy with how this method is setup, the contents are fine, but the design
			time::DurationMilliseconds duration = time::SteadyClock::now() - _start_timestamp;
			flt seconds = duration.count() / 1000.0f;
			_meta_values.Model =
				glm::rotate(glm::mat4(1.0f), seconds * glm::radians(90.0f) / 4.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		}

		void RenderToFrame(Frame& frame, Pipeline& pipeline) override
		{
			VulkanFrame& vulkan_frame = (VulkanFrame&)frame;
			VulkanPipeline& vulkan_pipeline = (VulkanPipeline&)pipeline;

			PrepareForPipeline(pipeline);
			UpdateMetaValues();

			vulkan_frame.StageCommand(*_push_constants);
			vulkan_frame.StageCommand(*_bind_vertex_buffers);
			vulkan_frame.StageCommand(*_bind_index_buffer);
			vulkan_frame.StageCommand(*_draw_indexed);
		}

		void PrepareForPipeline(Pipeline& pipeline) override
		{
			VulkanPipeline& vulkan_pipeline = (VulkanPipeline&)pipeline;
			VulkanDevice& vulkan_device = vulkan_pipeline.GetDevice();

			if (IsOutOfDate() || true) // TODO: remove "|| true"
			{
				DisposeForPipeline(pipeline);

				// create texture
				VkImageCreateInfo texture_image_create_info = VulkanImage::CreateInfo();
				texture_image_create_info.extent.width = _model.GetTextureImage().GetWidth();
				texture_image_create_info.extent.height = _model.GetTextureImage().GetHeight();
				texture_image_create_info.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
				VkMemoryPropertyFlags texture_memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
				VkImageViewCreateInfo texture_image_view_create_info = VulkanImageView::CreateInfo();
				_texture = CreateTexture(vulkan_device, texture_image_create_info, texture_memory_property_flags,
										 texture_image_view_create_info);

				// create staging buffer
				VulkanBuffer* staging =
					CreateBuffer(vulkan_device, _model.GetTextureImage().Size(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				// copy image data to staging
				void* data;
				vkMapMemory(vulkan_device, staging->GetDeviceMemory(), 0, _model.GetTextureImage().Size(), 0, &data);
				memcpy(data, _model.GetTextureImage().Data(), _model.GetTextureImage().Size()); // TODO: include needed header
				vkUnmapMemory(vulkan_device, staging->GetDeviceMemory());

				// copy staging to texture
				_texture->GetImage().TransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
													  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

				VulkanImage::Copy(_texture->GetImage(), *staging, _model.GetTextureImage().GetWidth(),
								  _model.GetTextureImage().GetHeight());

				_texture->GetImage().TransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
													  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

				memory::Destroy<VulkanBuffer>(memory::DefaultTraitAllocator, staging);

				// create vertex buffer
				VkDeviceSize data_size = sizeof(VulkanVertex) * _model.GetVertices().size();

				staging = CreateBuffer(vulkan_device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
									   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				_vertex_buffer =
					CreateBuffer(vulkan_device, data_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

				vkMapMemory(vulkan_device, staging->GetDeviceMemory(), 0, data_size, 0, &data);
				memcpy(data, _model.GetVertices().data(), data_size);
				vkUnmapMemory(vulkan_device, staging->GetDeviceMemory());

				VulkanBuffer::Copy(*_vertex_buffer, *staging, data_size);
				memory::Destroy<VulkanBuffer>(memory::DefaultTraitAllocator, staging);

				// create index buffer
				data_size = sizeof(ui32) * _model.GetIndices().size();

				staging = CreateBuffer(vulkan_device, data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
									   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				_index_buffer =
					CreateBuffer(vulkan_device, data_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

				vkMapMemory(vulkan_device, staging->GetDeviceMemory(), 0, data_size, 0, &data);
				memcpy(data, _model.GetIndices().data(), data_size);
				vkUnmapMemory(vulkan_device, staging->GetDeviceMemory());

				VulkanBuffer::Copy(*_index_buffer, *staging, data_size);
				memory::Destroy<VulkanBuffer>(memory::DefaultTraitAllocator, staging);

				// create commands
				_vk_vertex_buffer = *_vertex_buffer;

				_push_constants = memory::Create<VulkanCommandPushConstants>(
					memory::DefaultTraitAllocator, vulkan_pipeline.GetPipelineLayout(),
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(ObjectMetaValues), &_meta_values);

				_bind_vertex_buffers = memory::Create<VulkanCommandBindVertexBuffers>(
					memory::DefaultTraitAllocator, 0, 1, &_vk_vertex_buffer, _vertex_offsets.data());

				_bind_index_buffer = memory::Create<VulkanCommandBindIndexBuffer>(memory::DefaultTraitAllocator, *_index_buffer,
																				  0, VK_INDEX_TYPE_UINT32);

				_draw_indexed = memory::Create<VulkanCommandDrawIndexed>(memory::DefaultTraitAllocator,
																		 (ui32)_model.GetIndices().size(), 1, 0, 0, 0);

				// SetOutOfDate(false); //TODO:
			}

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