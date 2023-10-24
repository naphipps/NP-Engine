//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/26/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_CONTEXT_HPP
#define NP_ENGINE_VULKAN_RENDER_CONTEXT_HPP

#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"
#include "VulkanRenderFrame.hpp"

namespace np::gpu::__detail
{
	class VulkanRenderContext : public RenderContext
	{
	private:
		VkExtent2D _extent;
		VkSwapchainKHR _swapchain;
		con::vector<VulkanRenderFrame> _frames;
		ui32 _current_frame_index;
		ui32 _next_frame_index;

		static VkSwapchainCreateInfoKHR CreateSwapchainInfo(mem::sptr<RenderDevice> device)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			VulkanRenderTarget& target = (VulkanRenderTarget&)(*device->GetRenderTarget());

			VkSwapchainCreateInfoKHR info{};
			info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			info.surface = target;
			info.imageFormat = render_device.GetSurfaceFormat().format;
			info.imageColorSpace = render_device.GetSurfaceFormat().colorSpace;
			info.imageArrayLayers = 1;
			info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			return info;
		}

		static VkImageViewCreateInfo CreateImageViewInfo(mem::sptr<RenderDevice> device)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);

			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = render_device.GetSurfaceFormat().format;
			info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			info.subresourceRange.baseMipLevel = 0;
			info.subresourceRange.levelCount = 1;
			info.subresourceRange.baseArrayLayer = 0;
			info.subresourceRange.layerCount = 1;
			return info;
		}

		static VkExtent2D ChooseExtent(mem::sptr<RenderDevice> device)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			VulkanRenderTarget& target = (VulkanRenderTarget&)(*device->GetRenderTarget());
			VkSurfaceCapabilitiesKHR capabilities{};

			if (render_device.GetPhysicalDevice())
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(render_device.GetPhysicalDevice(), target, &capabilities);

			VkExtent2D extent{};

			if (capabilities.currentExtent.width != UI32_MAX)
			{
				extent = capabilities.currentExtent;
			}
			else
			{
				VkExtent2D framebuffer_extent = target.GetFramebufferExtent();
				VkExtent2D min_extent = capabilities.minImageExtent;
				VkExtent2D max_extent = capabilities.maxImageExtent;

				extent = {::std::clamp(framebuffer_extent.width, min_extent.width, max_extent.width),
						  ::std::clamp(framebuffer_extent.height, min_extent.height, max_extent.height)};
			}

			return extent;
		}

		static VkSwapchainKHR CreateSwapchain(mem::sptr<RenderDevice> device)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			VulkanRenderTarget& target = (VulkanRenderTarget&)(*device->GetRenderTarget());
			VkSwapchainKHR swapchain = nullptr;

			VkSwapchainCreateInfoKHR swapchain_info = CreateSwapchainInfo(device);
			swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchain_info.presentMode = render_device.GetPresentMode();
			swapchain_info.clipped = VK_TRUE;
			swapchain_info.oldSwapchain = nullptr;

			con::vector<ui32> indices{render_device.GetGraphicsFamilyIndex(), render_device.GetPresentFamilyIndex()};

			if (render_device.GetGraphicsFamilyIndex() != render_device.GetPresentFamilyIndex())
			{
				swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				swapchain_info.queueFamilyIndexCount = (ui32)indices.size();
				swapchain_info.pQueueFamilyIndices = indices.data();
			}
			else
			{
				swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			}

			VkSurfaceCapabilitiesKHR surface_capabilities{};

			if (render_device.GetPhysicalDevice())
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(render_device.GetPhysicalDevice(), target, &surface_capabilities);

			swapchain_info.imageExtent = ChooseExtent(device);
			swapchain_info.preTransform = surface_capabilities.currentTransform; // says that we don't want any local transform
			swapchain_info.minImageCount = 3; // TODO: NO - figure out best way to set this

			if (surface_capabilities.maxImageCount != 0)
				swapchain_info.minImageCount = ::std::min(swapchain_info.minImageCount, surface_capabilities.maxImageCount);

			if (vkCreateSwapchainKHR(render_device, &swapchain_info, nullptr, &swapchain) != VK_SUCCESS)
				swapchain = nullptr;

			return swapchain;
		}

		static con::vector<VkImage> RetrieveImages(mem::sptr<RenderDevice> device, VkSwapchainKHR swapchain)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			con::vector<VkImage> images;

			if (swapchain)
			{
				ui32 count;
				vkGetSwapchainImagesKHR(render_device, swapchain, &count, nullptr);
				images.resize(count);
				vkGetSwapchainImagesKHR(render_device, swapchain, &count, images.data());
			}

			return images;
		}

		static con::vector<VulkanRenderFrame> CreateFrames(mem::sptr<RenderDevice> device, VkSwapchainKHR swapchain)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			con::vector<VkImage> images = RetrieveImages(device, swapchain);
			VkImageViewCreateInfo image_view_info = VulkanImageView::CreateInfo();
			image_view_info.format = render_device.GetSurfaceFormat().format;

			con::vector<VulkanRenderFrame> frames;
			for (siz i = 0; i < images.size(); i++)
			{
				image_view_info.image = images[i];
				frames.emplace_back(i, device, image_view_info);
			}

			return frames;
		}

		void DestroySwapchain()
		{
			if (_swapchain)
			{
				VulkanRenderDevice& device = (VulkanRenderDevice&)(*GetRenderDevice());
				vkDestroySwapchainKHR(device, _swapchain, nullptr);
				_swapchain = nullptr;
			}
		}

		void RebuildFrames()
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*_device);
			con::vector<VkImage> images = RetrieveImages(_device, _swapchain);
			VkImageViewCreateInfo image_view_info = VulkanImageView::CreateInfo();
			image_view_info.format = render_device.GetSurfaceFormat().format;
			mem::sptr<VulkanLogicalDevice> logical_device = render_device.GetLogicalDevice();

			while (images.size() < _frames.size())
				_frames.pop_back();

			for (siz i = 0; i < images.size(); i++)
			{
				image_view_info.image = images[i];

				if (i < _frames.size())
				{
					_frames[i].image = images[i];
					_frames[i].imageView = VulkanImageView{logical_device, image_view_info};
					_frames[i].prevFrameFence = nullptr;
				}
				else
				{
					_frames.emplace_back(i, _device, image_view_info);
				}
			}
		}

	public:
		VulkanRenderContext(mem::sptr<RenderDevice> device):
			RenderContext(device),
			_extent(ChooseExtent(_device)),
			_swapchain(CreateSwapchain(_device)),
			_frames(CreateFrames(_device, _swapchain)),
			_current_frame_index(0),
			_next_frame_index(0)
		{}

		~VulkanRenderContext()
		{
			VulkanRenderDevice& device = (VulkanRenderDevice&)(*GetRenderDevice());
			vkDeviceWaitIdle(device);
			_frames.clear();
			DestroySwapchain();
		}

		siz GetFramesCount() const
		{
			return _frames.size();
		}

		VkExtent2D GetExtent() const
		{
			return _extent;
		}

		flt GetAspectRatio() const
		{
			return (flt)_extent.width / (flt)_extent.height;
		}

		VulkanRenderFrame& GetCurrentFrame()
		{
			return _frames[_current_frame_index];
		}

		const VulkanRenderFrame& GetCurrentFrame() const
		{
			return _frames[_current_frame_index];
		}

		VulkanRenderFrame& GetNextFrame()
		{
			return _frames[_next_frame_index];
		}

		const VulkanRenderFrame& GetNextFrame() const
		{
			return _frames[_next_frame_index];
		}

		void Rebuild()
		{
			DestroySwapchain();
			_extent = ChooseExtent(_device);
			_swapchain = CreateSwapchain(_device);
			RebuildFrames();
		}

		VkResult AcquireImage()
		{
			VulkanRenderDevice& device = (VulkanRenderDevice&)(*GetRenderDevice());
			_frames[_current_frame_index].fence.Wait();

			VkResult result = vkAcquireNextImageKHR(device, _swapchain, UI64_MAX, _frames[_current_frame_index].imageSemaphore,
													nullptr, &_next_frame_index);

			NP_ENGINE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR,
							 "vkAcquireNextImageKHR error");

			return result;
		}

		void MarkAcquiredImageForUse()
		{
			VulkanRenderDevice& device = (VulkanRenderDevice&)(*GetRenderDevice());

			// Check if a previous frame is using this image (i.e. there is its fence to wait on)
			if (_frames[_next_frame_index].prevFrameFence)
				_frames[_next_frame_index].prevFrameFence->Wait();

			// Mark the image as now being in use by this frame
			_frames[_next_frame_index].prevFrameFence = &_frames[_current_frame_index].fence;
		}

		void IncFrame()
		{
			_current_frame_index = (_current_frame_index + 1) % GetFramesCount();
		}

		const con::vector<VulkanRenderFrame>& GetFrames() const
		{
			return _frames;
		}

		con::vector<VulkanRenderFrame>& GetFrames()
		{
			return _frames;
		}

		operator VkSwapchainKHR() const
		{
			return _swapchain;
		}
	};

	using VulkanRenderSwapchain = VulkanRenderContext;
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_RENDER_CONTEXT_HPP */