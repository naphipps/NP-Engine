//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_BIND_DESCRIPTOR_SETS_HPP
#define NP_ENGINE_VULKAN_COMMAND_BIND_DESCRIPTOR_SETS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::gfx::rhi
{
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
										ui32* dynamic_offsets):
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
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_BIND_DESCRIPTOR_SETS_HPP */