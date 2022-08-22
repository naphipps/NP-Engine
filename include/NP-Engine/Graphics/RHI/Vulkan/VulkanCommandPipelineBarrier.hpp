//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_PIPELINE_BARRIER_HPP
#define NP_ENGINE_VULKAN_COMMAND_PIPELINE_BARRIER_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::gfx::rhi
{
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
									 ui32 image_memory_barrier_count, VkImageMemoryBarrier* image_memory_barriers):
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
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_PIPELINE_BARRIER_HPP */