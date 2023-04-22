//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMANDS_HPP
#define NP_ENGINE_VULKAN_COMMANDS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/Interface/Interface.hpp"

namespace np::gfx::__detail
{
	struct VulkanCommand : public Command
	{
		GraphicsDetailType GetDetailType() const override
		{
			return GraphicsDetailType::Vulkan;
		}

		virtual void ApplyTo(VkCommandBuffer command_buffer) = 0;
	};

	class VulkanCommandBeginRenderPass : public VulkanCommand
	{
	public:
		VkRenderPassBeginInfo RenderPassBeginInfo;
		VkSubpassContents SubpassContents;

		VulkanCommandBeginRenderPass(VkRenderPassBeginInfo render_pass_begin_info, VkSubpassContents subpass_contents) :
			RenderPassBeginInfo(render_pass_begin_info),
			SubpassContents(subpass_contents)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBeginRenderPass(command_buffer, &RenderPassBeginInfo, SubpassContents);
		}
	};

	class VulkanCommandBindDescriptorSets : public VulkanCommand
	{
	public:
		VkPipelineBindPoint PipelineBindPoint;
		VkPipelineLayout PipelineLayout;
		ui32 FirstSet;
		ui32 DescriptorSetCount;
		VkDescriptorSet* DescriptorSets;
		ui32 DynamicOffsetCount;
		ui32* DynamicOffsets;

		VulkanCommandBindDescriptorSets(VkPipelineBindPoint pipeline_bind_point, VkPipelineLayout layout, ui32 first_set,
			ui32 descriptor_set_count, VkDescriptorSet* descriptor_sets, ui32 dynamic_offset_count,
			ui32* dynamic_offsets) :
			PipelineBindPoint(pipeline_bind_point),
			PipelineLayout(layout),
			FirstSet(first_set),
			DescriptorSetCount(descriptor_set_count),
			DescriptorSets(descriptor_sets),
			DynamicOffsetCount(dynamic_offset_count),
			DynamicOffsets(dynamic_offsets)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindDescriptorSets(command_buffer, PipelineBindPoint, PipelineLayout, FirstSet, DescriptorSetCount,
				DescriptorSets, DynamicOffsetCount, DynamicOffsets);
		}
	};

	class VulkanCommandBindIndexBuffer : public VulkanCommand
	{
	public:
		VkBuffer Buffer;
		VkDeviceSize Offset;
		VkIndexType IndexType;

		VulkanCommandBindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type) :
			Buffer(buffer),
			Offset(offset),
			IndexType(index_type)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindIndexBuffer(command_buffer, Buffer, Offset, IndexType);
		}
	};

	class VulkanCommandBindPipeline : public VulkanCommand
	{
	public:
		VkPipelineBindPoint PipelineBindPoint;
		VkPipeline Pipeline;

		VulkanCommandBindPipeline(VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline) :
			PipelineBindPoint(pipeline_bind_point),
			Pipeline(pipeline)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindPipeline(command_buffer, PipelineBindPoint, Pipeline);
		}
	};

	class VulkanCommandBindVertexBuffers : public VulkanCommand
	{
	public:
		ui32 FirstBinding;
		ui32 BindingCount;
		VkBuffer* Buffers;
		VkDeviceSize* Offsets;

		VulkanCommandBindVertexBuffers(ui32 first_binding, ui32 binding_count, VkBuffer* buffers, VkDeviceSize* offsets) :
			FirstBinding(first_binding),
			BindingCount(binding_count),
			Buffers(buffers),
			Offsets(offsets)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindVertexBuffers(command_buffer, FirstBinding, BindingCount, Buffers, Offsets);
		}
	};

	class VulkanCommandDrawIndexed : public VulkanCommand
	{
	public:
		ui32 IndexCount;
		ui32 InstanceCount;
		ui32 FirstIndex;
		i32 VertexOffset;
		ui32 FirstInstance;

		VulkanCommandDrawIndexed(ui32 index_count, ui32 instance_count, ui32 first_index, i32 vertex_offset,
			ui32 first_instance) :
			IndexCount(index_count),
			InstanceCount(instance_count),
			FirstIndex(first_index),
			VertexOffset(vertex_offset),
			FirstInstance(first_instance)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdDrawIndexed(command_buffer, IndexCount, InstanceCount, FirstIndex, VertexOffset, FirstInstance);
		}
	};

	class VulkanCommandEndRenderPass : public VulkanCommand
	{
	public:
		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdEndRenderPass(command_buffer);
		}
	};

	class VulkanCommandPipelineBarrier : public VulkanCommand
	{
	public:
		VkPipelineStageFlags SrcPipelineStageFlags;
		VkPipelineStageFlags DstPipelineStageFlags;
		VkDependencyFlags DependencyFlags;
		ui32 MemoryBarrierCount;
		VkMemoryBarrier* MemoryBarriers;
		ui32 BufferMemoryBarrierCount;
		VkBufferMemoryBarrier* BufferMemoryBarriers;
		ui32 ImageMemoryBarrierCount;
		VkImageMemoryBarrier* ImageMemoryBarriers;

		VulkanCommandPipelineBarrier(VkPipelineStageFlags src_pipeline_stage_flags,
			VkPipelineStageFlags dst_pipeline_stage_flags, VkDependencyFlags dependency_flags,
			ui32 memory_barrier_count, VkMemoryBarrier* memory_barriers,
			ui32 buffer_memory_barrier_count, VkBufferMemoryBarrier* buffer_memory_barriers,
			ui32 image_memory_barrier_count, VkImageMemoryBarrier* image_memory_barriers) :
			SrcPipelineStageFlags(src_pipeline_stage_flags),
			DstPipelineStageFlags(dst_pipeline_stage_flags),
			DependencyFlags(dependency_flags),
			MemoryBarrierCount(memory_barrier_count),
			MemoryBarriers(memory_barriers),
			BufferMemoryBarrierCount(buffer_memory_barrier_count),
			BufferMemoryBarriers(buffer_memory_barriers),
			ImageMemoryBarrierCount(image_memory_barrier_count),
			ImageMemoryBarriers(image_memory_barriers)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdPipelineBarrier(command_buffer, SrcPipelineStageFlags, DstPipelineStageFlags, DependencyFlags,
				MemoryBarrierCount, MemoryBarriers, BufferMemoryBarrierCount, BufferMemoryBarriers,
				ImageMemoryBarrierCount, ImageMemoryBarriers);
		}
	};

	class VulkanCommandCopyBuffers : public VulkanCommand
	{
	public:
		VkBuffer SrcBuffer;
		VkBuffer DstBuffer;
		ui32 RegionCount;
		VkBufferCopy* Regions;

		VulkanCommandCopyBuffers(VkBuffer src_buffer, VkBuffer dst_buffer, ui32 region_count, VkBufferCopy* regions) :
			SrcBuffer(src_buffer),
			DstBuffer(dst_buffer),
			RegionCount(region_count),
			Regions(regions)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdCopyBuffer(command_buffer, SrcBuffer, DstBuffer, RegionCount, Regions);
		}
	};

	class VulkanCommandCopyBufferToImage : public VulkanCommand
	{
	public:
		VkBuffer SrcBuffer;
		VkImage DstImage;
		VkImageLayout DstImageLayout;
		ui32 RegionCount;
		VkBufferImageCopy* Regions;

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
			buffer_image_copy.imageOffset = { 0, 0, 0 };
			// buffer_image_copy.imageExtent = { width, height, 1 };
			return buffer_image_copy;
		}

		VulkanCommandCopyBufferToImage(VkBuffer src_buffer, VkImage dst_image, VkImageLayout dst_image_layout,
			ui32 region_count, VkBufferImageCopy* regions) :
			SrcBuffer(src_buffer),
			DstImage(dst_image),
			DstImageLayout(dst_image_layout),
			RegionCount(region_count),
			Regions(regions)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdCopyBufferToImage(command_buffer, SrcBuffer, DstImage, DstImageLayout, RegionCount, Regions);
		}
	};

	class VulkanCommandPushConstants : public VulkanCommand
	{
	public:
		VkPipelineLayout PipelineLayout;
		VkShaderStageFlags ShaderStageFlags;
		ui32 Offset;
		ui32 Size;
		void* Values;

		VulkanCommandPushConstants(VkPipelineLayout pipeline_layout, VkShaderStageFlags shader_stage_flags, ui32 offset,
			ui32 size, void* values) :
			PipelineLayout(pipeline_layout),
			ShaderStageFlags(shader_stage_flags),
			Offset(offset),
			Size(size),
			Values(values)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdPushConstants(command_buffer, PipelineLayout, ShaderStageFlags, Offset, Size, Values);
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_COMMANDS_HPP */