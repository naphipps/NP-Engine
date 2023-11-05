//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMANDS_HPP
#define NP_ENGINE_VULKAN_COMMANDS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

namespace np::gpu::__detail
{
	struct VulkanCommand : public Command
	{
		DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual void ApplyTo(VkCommandBuffer command_buffer) = 0;
	};

	struct VulkanCommandBeginRenderPass : public VulkanCommand
	{
		VkRenderPassBeginInfo renderPassBeginInfo;
		VkSubpassContents subpassContents;

		VulkanCommandBeginRenderPass(VkRenderPassBeginInfo render_pass_begin_info, VkSubpassContents subpass_contents):
			renderPassBeginInfo(render_pass_begin_info),
			subpassContents(subpass_contents)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBeginRenderPass(command_buffer, &renderPassBeginInfo, subpassContents);
		}
	};

	struct VulkanCommandBindDescriptorSets : public VulkanCommand
	{
		VkPipelineBindPoint pipelineBindPoint;
		VkPipelineLayout pipelineLayout;
		ui32 firstSet;
		ui32 descriptorSetCount;
		VkDescriptorSet* descriptorSets;
		ui32 dynamicOffsetCount;
		ui32* dynamicOffsets;

		VulkanCommandBindDescriptorSets(VkPipelineBindPoint pipeline_bind_point, VkPipelineLayout layout, ui32 first_set,
										ui32 descriptor_set_count, VkDescriptorSet* descriptor_sets, ui32 dynamic_offset_count,
										ui32* dynamic_offsets):
			pipelineBindPoint(pipeline_bind_point),
			pipelineLayout(layout),
			firstSet(first_set),
			descriptorSetCount(descriptor_set_count),
			descriptorSets(descriptor_sets),
			dynamicOffsetCount(dynamic_offset_count),
			dynamicOffsets(dynamic_offsets)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindDescriptorSets(command_buffer, pipelineBindPoint, pipelineLayout, firstSet, descriptorSetCount,
									descriptorSets, dynamicOffsetCount, dynamicOffsets);
		}
	};

	struct VulkanCommandBindIndexBuffer : public VulkanCommand
	{
		VkBuffer buffer;
		VkDeviceSize offset;
		VkIndexType indexType;

		VulkanCommandBindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type):
			buffer(buffer),
			offset(offset),
			indexType(index_type)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindIndexBuffer(command_buffer, buffer, offset, indexType);
		}
	};

	struct VulkanCommandBindPipeline : public VulkanCommand
	{
		VkPipelineBindPoint pipelineBindPoint;
		VkPipeline pipeline;

		VulkanCommandBindPipeline(VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline):
			pipelineBindPoint(pipeline_bind_point),
			pipeline(pipeline)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindPipeline(command_buffer, pipelineBindPoint, pipeline);
		}
	};

	struct VulkanCommandBindVertexBuffers : public VulkanCommand
	{
		ui32 firstBinding;
		ui32 bindingCount;
		VkBuffer* buffers;
		VkDeviceSize* offsets;

		VulkanCommandBindVertexBuffers(ui32 first_binding, ui32 binding_count, VkBuffer* buffers, VkDeviceSize* offsets):
			firstBinding(first_binding),
			bindingCount(binding_count),
			buffers(buffers),
			offsets(offsets)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindVertexBuffers(command_buffer, firstBinding, bindingCount, buffers, offsets);
		}
	};

	struct VulkanCommandDrawIndexed : public VulkanCommand
	{
		ui32 indexCount;
		ui32 instanceCount;
		ui32 firstIndex;
		i32 vertexOffset;
		ui32 firstInstance;

		VulkanCommandDrawIndexed(ui32 index_count, ui32 instance_count, ui32 first_index, i32 vertex_offset,
								 ui32 first_instance):
			indexCount(index_count),
			instanceCount(instance_count),
			firstIndex(first_index),
			vertexOffset(vertex_offset),
			firstInstance(first_instance)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdDrawIndexed(command_buffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}
	};

	struct VulkanCommandEndRenderPass : public VulkanCommand
	{
		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdEndRenderPass(command_buffer);
		}
	};

	struct VulkanCommandPipelineBarrier : public VulkanCommand
	{
		VkPipelineStageFlags dstPipelineStageFlags;
		VkPipelineStageFlags srcPipelineStageFlags;
		VkDependencyFlags dependencyFlags;
		ui32 memoryBarrierCount;
		VkMemoryBarrier* memoryBarriers;
		ui32 bufferMemoryBarrierCount;
		VkBufferMemoryBarrier* bufferMemoryBarriers;
		ui32 imageMemoryBarrierCount;
		VkImageMemoryBarrier* imageMemoryBarriers;

		VulkanCommandPipelineBarrier(VkPipelineStageFlags dst_pipeline_stage_flags,
									 VkPipelineStageFlags src_pipeline_stage_flags, VkDependencyFlags dependency_flags,
									 ui32 memory_barrier_count, VkMemoryBarrier* memory_barriers,
									 ui32 buffer_memory_barrier_count, VkBufferMemoryBarrier* buffer_memory_barriers,
									 ui32 image_memory_barrier_count, VkImageMemoryBarrier* image_memory_barriers):
			dstPipelineStageFlags(dst_pipeline_stage_flags),
			srcPipelineStageFlags(src_pipeline_stage_flags),
			dependencyFlags(dependency_flags),
			memoryBarrierCount(memory_barrier_count),
			memoryBarriers(memory_barriers),
			bufferMemoryBarrierCount(buffer_memory_barrier_count),
			bufferMemoryBarriers(buffer_memory_barriers),
			imageMemoryBarrierCount(image_memory_barrier_count),
			imageMemoryBarriers(image_memory_barriers)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdPipelineBarrier(command_buffer, srcPipelineStageFlags, dstPipelineStageFlags, dependencyFlags,
								 memoryBarrierCount, memoryBarriers, bufferMemoryBarrierCount, bufferMemoryBarriers,
								 imageMemoryBarrierCount, imageMemoryBarriers);
		}
	};

	struct VulkanCommandCopyBuffers : public VulkanCommand
	{
		VkBuffer dstBuffer;
		VkBuffer srcBuffer;
		ui32 regionCount;
		VkBufferCopy* regions;

		VulkanCommandCopyBuffers(VkBuffer dst_buffer, VkBuffer src_buffer, ui32 region_count, VkBufferCopy* regions):
			dstBuffer(dst_buffer),
			srcBuffer(src_buffer),
			regionCount(region_count),
			regions(regions)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdCopyBuffer(command_buffer, srcBuffer, dstBuffer, regionCount, regions);
		}
	};

	struct VulkanCommandCopyBufferToImage : public VulkanCommand
	{
		VkImage dstImage;
		VkBuffer srcBuffer;
		VkImageLayout dstImageLayout;
		ui32 regionCount;
		VkBufferImageCopy* regions;

		static VkBufferImageCopy CreateBufferImageCopy()
		{
			VkBufferImageCopy buffer_image_copy{};
			buffer_image_copy.bufferOffset = 0;
			buffer_image_copy.bufferRowLength = 0;
			buffer_image_copy.bufferImageHeight = 0;
			buffer_image_copy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			buffer_image_copy.imageSubresource.mipLevel = 0;
			buffer_image_copy.imageSubresource.baseArrayLayer = 0;
			buffer_image_copy.imageSubresource.layerCount = 1;
			buffer_image_copy.imageOffset = {0, 0, 0};
			// buffer_image_copy.imageExtent = { width, height, 1 };
			return buffer_image_copy;
		}

		VulkanCommandCopyBufferToImage(VkImage dst_image, VkBuffer src_buffer, VkImageLayout dst_image_layout,
									   ui32 region_count, VkBufferImageCopy* regions):
			dstImage(dst_image),
			srcBuffer(src_buffer),
			dstImageLayout(dst_image_layout),
			regionCount(region_count),
			regions(regions)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdCopyBufferToImage(command_buffer, srcBuffer, dstImage, dstImageLayout, regionCount, regions);
		}
	};

	struct VulkanCommandPushConstants : public VulkanCommand
	{
		VkPipelineLayout pipelineLayout;
		VkShaderStageFlags shaderStageFlags;
		ui32 offset;
		ui32 size;
		void* values;

		VulkanCommandPushConstants(VkPipelineLayout pipeline_layout, VkShaderStageFlags shader_stage_flags, ui32 offset,
								   ui32 size, void* values):
			pipelineLayout(pipeline_layout),
			shaderStageFlags(shader_stage_flags),
			offset(offset),
			size(size),
			values(values)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdPushConstants(command_buffer, pipelineLayout, shaderStageFlags, offset, size, values);
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_COMMANDS_HPP */