//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_COPY_BUFFERS_HPP
#define NP_ENGINE_VULKAN_COMMAND_COPY_BUFFERS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::graphics::rhi
{
	class VulkanCommandCopyBuffers : public VulkanCommand
	{
	public:
		VkBuffer SrcBuffer;
		VkBuffer DstBuffer;
		ui32 RegionCount;
		VkBufferCopy* Regions;

		VulkanCommandCopyBuffers(VkBuffer src_buffer, VkBuffer dst_buffer, ui32 region_count, VkBufferCopy* regions):
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
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_COPY_BUFFERS_HPP */
