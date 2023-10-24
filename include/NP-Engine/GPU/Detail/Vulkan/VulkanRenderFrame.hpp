//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/9/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_FRAME_HPP
#define NP_ENGINE_VULKAN_RENDER_FRAME_HPP

#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanLogicalDevice.hpp"
#include "VulkanImageView.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"

namespace np::gpu::__detail
{
	struct VulkanRenderFrame
	{
		siz index;
		mem::sptr<RenderDevice> device;
		VkImage image; // we do not have device memory for these so we use VkImage type
		VulkanImageView imageView;
		VulkanSemaphore imageSemaphore;
		VulkanSemaphore renderSemaphore;
		VulkanFence fence;
		VulkanFence* prevFrameFence;
		mem::sptr<VulkanCommandBuffer> commandBuffer;

		VulkanRenderFrame(siz index, mem::sptr<RenderDevice> device, VkImageViewCreateInfo& image_view_info):
			index(index),
			device(device),
			image(image_view_info.image),
			imageView(GetLogicalDevice(), image_view_info),
			imageSemaphore(GetLogicalDevice()),
			renderSemaphore(GetLogicalDevice()),
			fence(GetLogicalDevice()),
			prevFrameFence(nullptr),
			commandBuffer(((VulkanCommandPool&)*((VulkanRenderDevice&)*device).GetCommandPool()).AllocateCommandBuffer())
		{}

		VulkanRenderFrame(VulkanRenderFrame&& other) noexcept:
			index(::std::move(other.index)),
			device(::std::move(other.device)),
			image(::std::move(other.image)),
			imageView(::std::move(other.imageView)),
			imageSemaphore(::std::move(other.imageSemaphore)),
			renderSemaphore(::std::move(other.renderSemaphore)),
			fence(::std::move(other.fence)),
			prevFrameFence(::std::move(other.prevFrameFence)),
			commandBuffer(::std::move(other.commandBuffer))
		{
			other.index = -1;
			other.device.reset();
			other.image = nullptr;
			other.prevFrameFence = nullptr;
			other.commandBuffer.reset();
		}

		~VulkanRenderFrame()
		{
			if (device && commandBuffer)
			{
				VulkanRenderDevice& render_device = (VulkanRenderDevice&)*device;
				VulkanCommandPool& command_pool = (VulkanCommandPool&)*render_device.GetCommandPool();
				command_pool.FreeCommandBuffer(commandBuffer);
			}
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)*device;
			return render_device.GetLogicalDevice();
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_RENDER_FRAME_HPP */