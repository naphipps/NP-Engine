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
