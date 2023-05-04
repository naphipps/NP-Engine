//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/26/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_CONTEXT_HPP
#define NP_ENGINE_VULKAN_RENDER_CONTEXT_HPP

#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/Interface/Interface.hpp"

#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanRenderDevice.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"

namespace np::gpu::__detail
{
	class VulkanRenderContext : public RenderContext
	{
	public:
		struct Frame
		{
			VulkanImage image;
			VulkanImageView imageView;
			mem::sptr<Framebuffers> framebuffers;
			mem::sptr<CommandBuffer> commandBuffer;
			VulkanSemaphore imageSemaphore;
			VulkanSemaphore presentSemaphore;
			VulkanFence inFlightFence;
		};

	private:
		VkExtent2D _extent;
		VkSwapchainKHR _swapchain;
		con::vector<VkImage> _images; // we do not have device memory for these so we'll keep VkImage type
		con::vector<VulkanImageView> _image_views;
		con::vector<VkSemaphore> _image_semaphores;
		con::vector<VkSemaphore> _render_semaphores;
		con::vector<VkFence> _fences;
		con::vector<VkFence> _acquired_image_fences;

		//con::vector<Frame> _frames;//TODO: I like this way better

		ui32 _current_image_index;
		ui32 _acquired_image_index;

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

				extent = { ::std::clamp(framebuffer_extent.width, min_extent.width, max_extent.width),
						  ::std::clamp(framebuffer_extent.height, min_extent.height, max_extent.height) };
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

			con::vector<ui32> indices{ render_device.GetGraphicsFamilyIndex(), render_device.GetPresentFamilyIndex() };

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
			swapchain_info.minImageCount = GetFramesInFlightCount();

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

		static con::vector<VulkanImageView> CreateImageViews(mem::sptr<RenderDevice> device, const con::vector<VkImage>& images)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			con::vector<VulkanImageView> image_views;

			VkImageViewCreateInfo info = VulkanImageView::CreateInfo();
			info.format = render_device.GetSurfaceFormat().format;

			for (const VkImage& image : images)
			{
				info.image = image;
				image_views.emplace_back(render_device.GetLogicalDevice(), info);
			}

			return image_views;
		}

		static con::vector<VkSemaphore> CreateSemaphores(mem::sptr<RenderDevice> device, siz count)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			con::vector<VkSemaphore> semaphores(count, nullptr);

			for (siz i = 0; i < count; i++)
			{
				VkSemaphoreCreateInfo semaphore_info{};
				semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

				if (vkCreateSemaphore(render_device, &semaphore_info, nullptr, &semaphores[i]) != VK_SUCCESS)
				{
					semaphores.clear();
					break;
				}
			}

			return semaphores;
		}

		static con::vector<VkFence> CreateFences(mem::sptr<RenderDevice> device, siz count)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			con::vector<VkFence> fences(count, nullptr);

			for (siz i = 0; i < count; i++)
			{
				VkFenceCreateInfo fence_info{};
				fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				if (vkCreateFence(render_device, &fence_info, nullptr, &fences[i]) != VK_SUCCESS)
				{
					fences.clear();
					break;
				}
			}

			return fences;
		}

		void Dispose()
		{
			_image_views.clear();
			_images.clear();
			_acquired_image_fences.clear();

			if (_swapchain)
			{
				VulkanRenderDevice& device = (VulkanRenderDevice&)(*GetRenderDevice());
				vkDestroySwapchainKHR(device, _swapchain, nullptr);
				_swapchain = nullptr;
			}
		}

	public:
		VulkanRenderContext(mem::sptr<RenderDevice> device) :
			RenderContext(device),
			_extent(ChooseExtent(device)),
			_swapchain(CreateSwapchain(device)),
			_images(RetrieveImages(device, _swapchain)),
			_image_views(CreateImageViews(device, _images)),
			_image_semaphores(CreateSemaphores(device, GetFramesInFlightCount())),
			_render_semaphores(CreateSemaphores(device, GetFramesInFlightCount())),
			_fences(CreateFences(device, GetFramesInFlightCount())),
			_acquired_image_fences(GetImages().size(), nullptr),
			_current_image_index(0),
			_acquired_image_index(0)
		{}

		~VulkanRenderContext()
		{
			VulkanRenderDevice& device = (VulkanRenderDevice&)(*GetRenderDevice());

			vkDeviceWaitIdle(device);

			for (VkSemaphore& semaphore : _render_semaphores)
				vkDestroySemaphore(device, semaphore, nullptr);

			_render_semaphores.clear();

			for (VkSemaphore& semaphore : _image_semaphores)
				vkDestroySemaphore(device, semaphore, nullptr);

			_image_semaphores.clear();

			for (VkFence& fence : _fences)
				vkDestroyFence(device, fence, nullptr);

			_fences.clear();

			Dispose();
		}

		static siz GetFramesInFlightCount() //TODO: I think we need to be able to change this per render context...
		{
			return 3;
		}

		VkExtent2D GetExtent() const
		{
			return _extent;
		}

		flt GetAspectRatio() const
		{
			return (flt)_extent.width / (flt)_extent.height;
		}

		ui32 GetCurrentImageIndex() const
		{
			return _current_image_index;
		}

		ui32 GetAcquiredImageIndex() const
		{
			return _acquired_image_index;
		}

		void Rebuild()
		{
			Dispose();

			_extent = ChooseExtent(_device);
			_swapchain = CreateSwapchain(_device);
			_images = RetrieveImages(_device, _swapchain);
			_image_views = CreateImageViews(_device, _images);

			_acquired_image_fences.resize(_images.size(), nullptr);
		}

		VkResult AcquireImage()
		{
			VulkanRenderDevice& device = (VulkanRenderDevice&)(*GetRenderDevice());
			vkWaitForFences(device, 1, &_fences[_current_image_index], VK_TRUE, UI64_MAX);

			VkResult result = vkAcquireNextImageKHR(device, _swapchain, UI64_MAX, _image_semaphores[_current_image_index],
				nullptr, &_acquired_image_index);

			NP_ENGINE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR, 
				"vkAcquireNextImageKHR error");

			return result;
		}

		void MarkAcquiredImageForUse()
		{
			VulkanRenderDevice& device = (VulkanRenderDevice&)(*GetRenderDevice());

			// Check if a previous frame is using this image (i.e. there is its fence to wait on)
			if (_acquired_image_fences[_acquired_image_index] != nullptr)
				vkWaitForFences(device, 1, &_acquired_image_fences[_acquired_image_index], VK_TRUE, UI64_MAX);

			// Mark the image as now being in use by this frame
			_acquired_image_fences[_acquired_image_index] = _fences[_current_image_index];
		}

		void IncCurrentImage()
		{
			_current_image_index = (_current_image_index + 1) % GetFramesInFlightCount();
		}

		const con::vector<VkImage>& GetImages() const
		{
			return _images;
		}

		const con::vector<VulkanImageView>& GetImageViews() const
		{
			return _image_views;
		}

		const con::vector<VkSemaphore> GetImageSemaphores() const
		{
			return _image_semaphores;
		}

		const con::vector<VkSemaphore> GetRenderSemaphores() const
		{
			return _render_semaphores;
		}

		const con::vector<VkFence> GetFences() const
		{
			return _fences;
		}

		const con::vector<VkFence> GetImageFences() const
		{
			return _acquired_image_fences;
		}

		operator VkSwapchainKHR() const
		{
			return _swapchain;
		}
	};

	using VulkanRenderSwapchain = VulkanRenderContext;
}

#endif /* NP_ENGINE_VULKAN_RENDER_CONTEXT_HPP */