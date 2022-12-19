//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_IMAGE_HPP
#define NP_ENGINE_VULKAN_IMAGE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanDevice.hpp"
#include "VulkanBuffer.hpp"

namespace np::gfx::__detail
{
	class VulkanImage
	{
	private:
		VulkanDevice& _device;
		VkImage _image;
		VkDeviceMemory _device_memory;

		VkImage CreateImage(VkImageCreateInfo& info)
		{
			VkImage image = nullptr;

			if (vkCreateImage(GetDevice(), &info, nullptr, &image) != VK_SUCCESS)
			{
				image = nullptr;
			}

			return image;
		}

		VkDeviceMemory CreateDeviceMemory(VkMemoryPropertyFlags memory_property_flags)
		{
			VkDeviceMemory device_memory = nullptr;

			if (_image)
			{
				VkMemoryRequirements requirements;
				vkGetImageMemoryRequirements(GetDevice(), _image, &requirements);

				VkMemoryAllocateInfo allocate_info{};
				allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocate_info.allocationSize = requirements.size;
				allocate_info.memoryTypeIndex =
					GetDevice().GetMemoryTypeIndex(requirements.memoryTypeBits, memory_property_flags);

				if (vkAllocateMemory(GetDevice(), &allocate_info, nullptr, &device_memory) != VK_SUCCESS)
				{
					device_memory = nullptr;
				}
				else if (vkBindImageMemory(GetDevice(), _image, device_memory, 0) != VK_SUCCESS)
				{
					device_memory = nullptr;
				}
			}

			return device_memory;
		}

	public:
		static VkImageCreateInfo CreateInfo()
		{
			VkImageCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.imageType = VK_IMAGE_TYPE_2D;
			info.extent = {0, 0, 1};
			info.format = VK_FORMAT_R8G8B8A8_SRGB;
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.mipLevels = 1;
			info.arrayLayers = 1;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			info.samples = VK_SAMPLE_COUNT_1_BIT;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			return info;
		}

		VulkanImage(VulkanDevice& device, VkImageCreateInfo& image_create_info, VkMemoryPropertyFlags memory_property_flags):
			_device(device),
			_image(CreateImage(image_create_info)),
			_device_memory(CreateDeviceMemory(memory_property_flags))
		{}

		VulkanImage(const VulkanImage&) = delete;

		VulkanImage(VulkanImage&& other) noexcept:
			_device(other._device),
			_image(::std::move(other._image)),
			_device_memory(::std::move(other._device_memory))
		{
			other._image = nullptr;
			other._device_memory = nullptr;
		}

		~VulkanImage()
		{
			if (_device_memory)
			{
				vkFreeMemory(GetDevice(), _device_memory, nullptr);
				_device_memory = nullptr;
			}

			if (_image)
			{
				vkDestroyImage(GetDevice(), _image, nullptr);
				_image = nullptr;
			}
		}

		operator VkImage() const
		{
			return _image;
		}

		VkResult AsyncAssign(VulkanBuffer& buffer, VkBufferImageCopy buffer_image_copy, VkSubmitInfo& submit_info,
							 con::vector<VulkanCommandBuffer>& command_buffers, VkFence fence = nullptr)
		{
			VulkanCommandCopyBufferToImage copy_buffer_to_image(buffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1,
																&buffer_image_copy);

			con::vector<VulkanCommandBuffer> buffers = GetDevice().AllocateCommandBuffers(1);
			VkCommandBufferBeginInfo begin_info = VulkanCommandBuffer::CreateBeginInfo();
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			GetDevice().BeginCommandBuffers(buffers, begin_info);
			buffers.front().Add(copy_buffer_to_image);
			GetDevice().EndCommandBuffers(buffers);
			for (auto it = buffers.begin(); it != buffers.end(); it++)
				command_buffers.emplace_back(*it);
			return GetDevice().GetGraphicsQueue().Submit(buffers, submit_info, fence);
		}

		VkResult SyncAssign(VulkanBuffer& buffer, VkBufferImageCopy buffer_image_copy, VkSubmitInfo& submit_info)
		{
			con::vector<VulkanCommandBuffer> command_buffers;
			VulkanFence fence(GetDevice());
			VkResult result = AsyncAssign(buffer, buffer_image_copy, submit_info, command_buffers, fence);
			fence.Wait();
			GetDevice().FreeCommandBuffers(command_buffers);
			return result;
		}

		VkDeviceMemory GetDeviceMemory() const
		{
			return _device_memory;
		}

		VulkanDevice& GetDevice() const
		{
			return _device;
		}

		VkResult AsyncTransitionLayout(VkFormat format, VkImageLayout old_image_layout, VkImageLayout new_image_layout,
									   VkSubmitInfo& submit_info, con::vector<VulkanCommandBuffer>& command_buffers,
									   VkFence fence = nullptr)
		{
			// TODO: figure out a way to include fence/semaphore parameters/returns/etc

			VkImageMemoryBarrier image_memory_barrier{};
			image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			image_memory_barrier.oldLayout = old_image_layout;
			image_memory_barrier.newLayout = new_image_layout;
			image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			image_memory_barrier.image = _image;
			image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_memory_barrier.subresourceRange.baseMipLevel = 0;
			image_memory_barrier.subresourceRange.levelCount = 1;
			image_memory_barrier.subresourceRange.baseArrayLayer = 0;
			image_memory_barrier.subresourceRange.layerCount = 1;

			VkPipelineStageFlags src_pipeline_stage_flags = 0;
			VkPipelineStageFlags dst_pipeline_stage_flags = 0;

			if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			{
				image_memory_barrier.srcAccessMask = 0;
				image_memory_barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				src_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dst_pipeline_stage_flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
			}
			else if (old_image_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
					 new_image_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				image_memory_barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				image_memory_barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				src_pipeline_stage_flags = VK_PIPELINE_STAGE_TRANSFER_BIT;
				dst_pipeline_stage_flags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}
			else if (old_image_layout == VK_IMAGE_LAYOUT_UNDEFINED &&
					 new_image_layout == VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL)
			{
				image_memory_barrier.srcAccessMask = 0;
				image_memory_barrier.dstAccessMask =
					VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				src_pipeline_stage_flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				dst_pipeline_stage_flags = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			}

			if (new_image_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL)
			{
				image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				if (VulkanHasStencilComponent(format))
				{
					image_memory_barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				}
			}
			else
			{
				image_memory_barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}

			VulkanCommandPipelineBarrier pipeline_barrier(src_pipeline_stage_flags, dst_pipeline_stage_flags, 0, 0, nullptr, 0,
														  nullptr, 1, &image_memory_barrier);

			con::vector<VulkanCommandBuffer> buffers = GetDevice().AllocateCommandBuffers(1);
			VkCommandBufferBeginInfo begin_info = VulkanCommandBuffer::CreateBeginInfo();
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			GetDevice().BeginCommandBuffers(buffers, begin_info);
			buffers.front().Add(pipeline_barrier);
			GetDevice().EndCommandBuffers(buffers);
			using I = con::vector<VulkanCommandBuffer>::iterator;
			for (I it = buffers.begin(); it != buffers.end(); it++)
				command_buffers.emplace_back(*it);
			return GetDevice().GetGraphicsQueue().Submit(buffers, submit_info, fence);
		}

		VkResult SyncTransitionLayout(VkFormat format, VkImageLayout old_image_layout, VkImageLayout new_image_layout,
									  VkSubmitInfo& submit_info)
		{
			con::vector<VulkanCommandBuffer> command_buffers;
			VulkanFence fence(GetDevice());
			VkResult result =
				AsyncTransitionLayout(format, old_image_layout, new_image_layout, submit_info, command_buffers, fence);
			fence.Wait();
			GetDevice().FreeCommandBuffers(command_buffers);
			return result;
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_IMAGE_HPP */