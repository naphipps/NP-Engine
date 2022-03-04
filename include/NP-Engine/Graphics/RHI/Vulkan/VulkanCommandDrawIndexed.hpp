//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_DRAW_INDEXED_HPP
#define NP_ENGINE_VULKAN_COMMAND_DRAW_INDEXED_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::graphics::rhi
{
	class VulkanCommandDrawIndexed : public VulkanCommand
	{
	public:
		ui32 IndexCount;
		ui32 InstanceCount;
		ui32 FirstIndex;
		i32 VertexOffset;
		ui32 FirstInstance;

		VulkanCommandDrawIndexed(ui32 index_count, ui32 instance_count, ui32 first_index, i32 vertex_offset,
								 ui32 first_instance):
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
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_DRAW_INDEXED_HPP */