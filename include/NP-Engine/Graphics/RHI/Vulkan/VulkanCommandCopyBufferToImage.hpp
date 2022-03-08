//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_COPY_BUFFER_TO_IMAGE_HPP
#define NP_ENGINE_VULKAN_COMMAND_COPY_BUFFER_TO_IMAGE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::graphics::rhi
{
	class VulkanCommandCopyBufferToImage : public VulkanCommand
	{
	public:
		VkBuffer SrcBuffer;
		VkImage DstImage;
		VkImageLayout DstImageLayout;
		ui32 RegionCount;
		VkBufferImageCopy* Regions;

		VulkanCommandCopyBufferToImage(VkBuffer src_buffer, VkImage dst_image, VkImageLayout dst_image_layout,
									   ui32 region_count, VkBufferImageCopy* regions):
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
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_COPY_BUFFER_TO_IMAGE_HPP */
