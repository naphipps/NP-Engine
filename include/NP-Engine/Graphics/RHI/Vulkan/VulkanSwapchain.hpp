//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SWAPCHAIN_HPP
#define NP_ENGINE_VULKAN_SWAPCHAIN_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanImageView.hpp"

namespace np::graphics::rhi
{
	class VulkanSwapchain
	{
	public:
		constexpr static ui32 MAX_FRAMES = 2;

	private:
		VulkanRenderPass& _render_pass;
		VkSwapchainKHR _swapchain;
		container::vector<VkImage> _images; // we do not have device memory for these so we'll keep VkImage type
		container::vector<VulkanImageView> _image_views;
		container::vector<VkFramebuffer> _framebuffers;
		container::vector<VkSemaphore> _image_available_semaphores; // TODO: refactor to _image_semaphores
		container::vector<VkSemaphore> _render_finished_semaphores; // TODO: return to _semaphores
		container::vector<VkFence> _fences;
		container::vector<VkFence> _image_fences;

		ui32 _current_image_index;
		ui32 _acquired_image_index;

		VkSwapchainCreateInfoKHR CreateSwapchainInfo()
		{
			VkSwapchainCreateInfoKHR info{};
			info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			info.surface = GetSurface();
			info.imageFormat = GetDevice().GetSurfaceFormat().format;
			info.imageColorSpace = GetDevice().GetSurfaceFormat().colorSpace;
			info.imageExtent = GetDevice().GetExtent();
			info.imageArrayLayers = 1;
			info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			return info;
		}

		VkImageViewCreateInfo CreateImageViewInfo() const
		{
			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = GetDevice().GetSurfaceFormat().format;
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

		VkFramebufferCreateInfo CreateFramebufferInfo()
		{
			VkFramebufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			info.width = GetDevice().GetExtent().width;
			info.height = GetDevice().GetExtent().height;
			info.layers = 1;
			return info;
		}

		VkSwapchainKHR CreateSwapchain()
		{
			VkSwapchainKHR swapchain = nullptr;

			ui32 min_image_count = GetDevice().GetCapabilities().minImageCount + 1;
			if (GetDevice().GetCapabilities().maxImageCount != 0)
				min_image_count = ::std::min(min_image_count, GetDevice().GetCapabilities().maxImageCount);

			container::vector<ui32> indices = GetDevice().GetQueueFamilyIndices().to_vector();

			VkSwapchainCreateInfoKHR swapchain_info = CreateSwapchainInfo();
			swapchain_info.minImageCount = min_image_count;

			if (GetDevice().GetQueueFamilyIndices().graphics != GetDevice().GetQueueFamilyIndices().present)
			{
				swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				swapchain_info.queueFamilyIndexCount = (ui32)indices.size();
				swapchain_info.pQueueFamilyIndices = indices.data();
			}
			else
			{
				swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			}

			// says that we don't want any local transform
			swapchain_info.preTransform = GetDevice().GetCapabilities().currentTransform;
			swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchain_info.presentMode = GetDevice().GetPresentMode();
			swapchain_info.clipped = VK_TRUE;
			swapchain_info.oldSwapchain = nullptr;

			if (vkCreateSwapchainKHR(GetDevice(), &swapchain_info, nullptr, &swapchain) != VK_SUCCESS)
			{
				swapchain = nullptr;
			}

			return swapchain;
		}

		container::vector<VkImage> RetrieveImages() const
		{
			container::vector<VkImage> images;

			if (_swapchain != nullptr)
			{
				ui32 count;
				vkGetSwapchainImagesKHR(GetDevice(), _swapchain, &count, nullptr);
				images.resize(count);
				vkGetSwapchainImagesKHR(GetDevice(), _swapchain, &count, images.data());
			}

			return images;
		}

		container::vector<VulkanImageView> CreateImageViews() // TODO: could we make use of ::std::move here??
		{
			container::vector<VulkanImageView> image_views;

			VkImageViewCreateInfo info = VulkanImageView::CreateInfo();
			info.format = GetDevice().GetSurfaceFormat().format;

			for (const VkImage& image : _images)
			{
				info.image = image;
				image_views.emplace_back(GetDevice(), info);
			}

			return image_views;
		}

		container::vector<VkFramebuffer> CreateFramebuffers()
		{
			container::vector<VkFramebuffer> framebuffers(GetImageViews().size());

			for (siz i = 0; i < GetImageViews().size(); i++)
			{
				container::vector<VkImageView> image_views{ GetImageViews()[i], GetRenderPass().GetDepthTexture().GetImageView()};

				VkFramebufferCreateInfo framebuffer_info = CreateFramebufferInfo();
				framebuffer_info.renderPass = GetRenderPass();
				framebuffer_info.attachmentCount = (ui32)image_views.size();
				framebuffer_info.pAttachments = image_views.data();

				if (vkCreateFramebuffer(GetDevice(), &framebuffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS)
				{
					framebuffers[i] = nullptr;
				}
			}

			return framebuffers;
		}

		container::vector<VkSemaphore> CreateSemaphores(siz count) const
		{
			container::vector<VkSemaphore> semaphores(count, nullptr);

			for (siz i = 0; i < count; i++)
			{
				VkSemaphoreCreateInfo semaphore_info{};
				semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				if (vkCreateSemaphore(GetDevice(), &semaphore_info, nullptr, &semaphores[i]) != VK_SUCCESS)
				{
					semaphores.clear();
					break;
				}
			}

			return semaphores;
		}

		container::vector<VkFence> CreateFences(siz count) const
		{
			container::vector<VkFence> fences(count, nullptr);

			for (siz i = 0; i < count; i++)
			{
				VkFenceCreateInfo fence_info{};
				fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
				fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

				if (vkCreateFence(GetDevice(), &fence_info, nullptr, &fences[i]) != VK_SUCCESS)
				{
					fences.clear();
					break;
				}
			}

			return fences;
		}

		void Dispose()
		{
			//_image_fences.clear(); // TODO: ???

			for (VkFramebuffer framebuffer : _framebuffers)
				vkDestroyFramebuffer(GetDevice(), framebuffer, nullptr);

			_framebuffers.clear();
			_image_views.clear();
			_images.clear();

			if (_swapchain) // TODO: we need this in every Vulkan class...
				vkDestroySwapchainKHR(GetDevice(), _swapchain, nullptr);
		}

	public:
		// TODO: I'd like to make all init methods const

		VulkanSwapchain(VulkanRenderPass& render_pass):
			_render_pass(render_pass),
			_swapchain(CreateSwapchain()),
			_images(RetrieveImages()), // TODO: _framebuffers, semaphores, fences
			_image_views(CreateImageViews()),
			_framebuffers(CreateFramebuffers()),
			_image_available_semaphores(CreateSemaphores(MAX_FRAMES)),
			_render_finished_semaphores(CreateSemaphores(MAX_FRAMES)),
			_fences(CreateFences(MAX_FRAMES)),
			_image_fences(GetImages().size(), nullptr),
			_current_image_index(0),
			_acquired_image_index(0)
		{}

		~VulkanSwapchain()
		{
			vkDeviceWaitIdle(GetDevice());

			for (VkSemaphore& semaphore : _render_finished_semaphores)
				vkDestroySemaphore(GetDevice(), semaphore, nullptr);

			for (VkSemaphore& semaphore : _image_available_semaphores)
				vkDestroySemaphore(GetDevice(), semaphore, nullptr);

			for (VkFence& fence : _fences)
				vkDestroyFence(GetDevice(), fence, nullptr);

			Dispose();
		}

		VulkanInstance& GetInstance()
		{
			return GetRenderPass().GetInstance();
		}

		const VulkanInstance& GetInstance() const
		{
			return GetRenderPass().GetInstance();
		}

		VulkanSurface& GetSurface()
		{
			return GetRenderPass().GetSurface();
		}

		const VulkanSurface& GetSurface() const
		{
			return GetRenderPass().GetSurface();
		}

		VulkanDevice& GetDevice()
		{
			return GetRenderPass().GetDevice();
		}

		const VulkanDevice& GetDevice() const
		{
			return GetRenderPass().GetDevice();
		}

		VulkanRenderPass& GetRenderPass()
		{
			return _render_pass;
		}

		const VulkanRenderPass& GetRenderPass() const
		{
			return _render_pass;
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

			_swapchain = CreateSwapchain();
			_images = RetrieveImages();
			_image_views = CreateImageViews();
			_framebuffers = CreateFramebuffers();
			//_image_fences.resize(_images.size(), nullptr); //TODO: we might need to keep this
		}

		// TODO: I'd like to just return a struct with image index and result
		VkResult AcquireImage()
		{
			vkWaitForFences(GetDevice(), 1, &_fences[_current_image_index], VK_TRUE, UI64_MAX);

			VkResult result =
				vkAcquireNextImageKHR(GetDevice(), _swapchain, UI64_MAX, _image_available_semaphores[_current_image_index],
									  nullptr, &_acquired_image_index);

			if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR &&
				result != VK_ERROR_OUT_OF_DATE_KHR)
			{
				NP_ENGINE_ASSERT(false, "vkAcquireNextImageKHR error");
			}

			return result;
		}

		void MarkAcquiredImageForUse()
		{
			// Check if a previous frame is using this image (i.e. there is its fence to wait on)
			if (_image_fences[_acquired_image_index] != nullptr)
			{
				vkWaitForFences(GetDevice(), 1, &_image_fences[_acquired_image_index], VK_TRUE, UI64_MAX);
			}

			// Mark the image as now being in use by this frame
			_image_fences[_acquired_image_index] = _fences[_current_image_index];
		}

		void IncCurrentImage()
		{
			_current_image_index = (_current_image_index + 1) % MAX_FRAMES;
		}

		const container::vector<VkImage>& GetImages() const
		{
			return _images;
		}

		const container::vector<VulkanImageView>& GetImageViews() const
		{
			return _image_views;
		}

		const container::vector<VkFramebuffer> GetFramebuffers() const
		{
			return _framebuffers;
		}

		const container::vector<VkSemaphore> GetImageAvailableSemaphores() const
		{
			return _image_available_semaphores;
		}

		const container::vector<VkSemaphore> GetRenderFinishedSemaphores() const
		{
			return _render_finished_semaphores;
		}

		const container::vector<VkFence> GetFences() const
		{
			return _fences;
		}

		const container::vector<VkFence> GetImageFences() const
		{
			return _image_fences;
		}

		operator VkSwapchainKHR() const
		{
			return _swapchain;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SWAPCHAIN_HPP */