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
#include "VulkanImageView.hpp"
#include "VulkanDevice.hpp"

namespace np::graphics::rhi
{
	class VulkanSwapchain
	{
	private:
		VulkanDevice& _device;
		VkExtent2D _extent;
		VkSwapchainKHR _swapchain;
		container::vector<VkImage> _images; // we do not have device memory for these so we'll keep VkImage type
		container::vector<VulkanImageView> _image_views;
		container::vector<VkSemaphore> _image_semaphores;
		container::vector<VkSemaphore> _render_semaphores;
		container::vector<VkFence> _fences;
		container::vector<VkFence> _acquired_image_fences;
		ui32 _current_image_index;
		ui32 _acquired_image_index;

		VkSwapchainCreateInfoKHR CreateSwapchainInfo()
		{
			VkSwapchainCreateInfoKHR info{};
			info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			info.surface = GetSurface();
			info.imageFormat = GetDevice().GetSurfaceFormat().format;
			info.imageColorSpace = GetDevice().GetSurfaceFormat().colorSpace;
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

		VkExtent2D ChooseExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
		{
			VkExtent2D extent{};

			if (capabilities.currentExtent.width != UI32_MAX)
			{
				extent = capabilities.currentExtent;
			}
			else
			{
				i32 width, height;
				glfwGetFramebufferSize((GLFWwindow*)GetSurface().GetWindow().GetNativeWindow(), &width, &height);

				VkExtent2D min_extent = capabilities.minImageExtent;
				VkExtent2D max_extent = capabilities.maxImageExtent;

				extent = {::std::clamp((ui32)width, min_extent.width, max_extent.width),
						  ::std::clamp((ui32)height, min_extent.height, max_extent.height)};
			}

			return extent;
		}

		VkSwapchainKHR CreateSwapchain()
		{
			VkSwapchainKHR swapchain = nullptr;

			VkSwapchainCreateInfoKHR swapchain_info = CreateSwapchainInfo();
			swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchain_info.presentMode = GetDevice().GetPresentMode();
			swapchain_info.clipped = VK_TRUE;
			swapchain_info.oldSwapchain = nullptr;

			container::vector<ui32> indices = GetDevice().GetQueueFamilyIndices().to_vector();

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

			VkSurfaceCapabilitiesKHR surface_capabilities{};

			if (GetDevice().GetPhysicalDevice() != nullptr)
				vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GetDevice().GetPhysicalDevice(), GetSurface(), &surface_capabilities);

			_extent = ChooseExtent(surface_capabilities);

			swapchain_info.imageExtent = _extent;
			swapchain_info.preTransform = surface_capabilities.currentTransform; // says that we don't want any local transform
			swapchain_info.minImageCount = NP_ENGINE_VULKAN_MAX_FRAME_COUNT;

			if (surface_capabilities.maxImageCount != 0)
				swapchain_info.minImageCount = ::std::min(swapchain_info.minImageCount, surface_capabilities.maxImageCount);

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

		container::vector<VulkanImageView> CreateImageViews()
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
			_image_views.clear();
			_images.clear();
			_acquired_image_fences.clear();

			if (_swapchain)
			{
				vkDestroySwapchainKHR(GetDevice(), _swapchain, nullptr);
				_swapchain = nullptr;
			}
		}

	public:
		VulkanSwapchain(VulkanDevice& device):
			_device(device),
			_swapchain(CreateSwapchain()),
			_images(RetrieveImages()),
			_image_views(CreateImageViews()),
			_image_semaphores(CreateSemaphores(NP_ENGINE_VULKAN_MAX_FRAME_COUNT)),
			_render_semaphores(CreateSemaphores(NP_ENGINE_VULKAN_MAX_FRAME_COUNT)),
			_fences(CreateFences(NP_ENGINE_VULKAN_MAX_FRAME_COUNT)),
			_acquired_image_fences(GetImages().size(), nullptr),
			_current_image_index(0),
			_acquired_image_index(0)
		{}

		~VulkanSwapchain()
		{
			vkDeviceWaitIdle(GetDevice());

			for (VkSemaphore& semaphore : _render_semaphores)
				vkDestroySemaphore(GetDevice(), semaphore, nullptr);

			for (VkSemaphore& semaphore : _image_semaphores)
				vkDestroySemaphore(GetDevice(), semaphore, nullptr);

			for (VkFence& fence : _fences)
				vkDestroyFence(GetDevice(), fence, nullptr);

			Dispose();
		}

		VulkanInstance& GetInstance()
		{
			return GetDevice().GetInstance();
		}

		const VulkanInstance& GetInstance() const
		{
			return GetDevice().GetInstance();
		}

		VulkanSurface& GetSurface()
		{
			return GetDevice().GetSurface();
		}

		const VulkanSurface& GetSurface() const
		{
			return GetDevice().GetSurface();
		}

		VulkanDevice& GetDevice()
		{
			return _device;
		}

		const VulkanDevice& GetDevice() const
		{
			return _device;
		}

		VkExtent2D GetExtent() const
		{
			return _extent;
		}

		flt GetAspectRatio()
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

			_swapchain = CreateSwapchain();
			_images = RetrieveImages();
			_image_views = CreateImageViews();

			_acquired_image_fences.resize(_images.size(), nullptr);
		}

		VkResult AcquireImage()
		{
			vkWaitForFences(GetDevice(), 1, &_fences[_current_image_index], VK_TRUE, UI64_MAX);

			VkResult result = vkAcquireNextImageKHR(GetDevice(), _swapchain, UI64_MAX, _image_semaphores[_current_image_index],
													nullptr, &_acquired_image_index);

			if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR && result != VK_ERROR_OUT_OF_DATE_KHR)
			{
				NP_ENGINE_ASSERT(false, "vkAcquireNextImageKHR error");
			}

			return result;
		}

		void MarkAcquiredImageForUse()
		{
			// Check if a previous frame is using this image (i.e. there is its fence to wait on)
			if (_acquired_image_fences[_acquired_image_index] != nullptr)
			{
				vkWaitForFences(GetDevice(), 1, &_acquired_image_fences[_acquired_image_index], VK_TRUE, UI64_MAX);
			}

			// Mark the image as now being in use by this frame
			_acquired_image_fences[_acquired_image_index] = _fences[_current_image_index];
		}

		void IncCurrentImage()
		{
			_current_image_index = (_current_image_index + 1) % NP_ENGINE_VULKAN_MAX_FRAME_COUNT;
		}

		const container::vector<VkImage>& GetImages() const
		{
			return _images;
		}

		const container::vector<VulkanImageView>& GetImageViews() const
		{
			return _image_views;
		}

		const container::vector<VkSemaphore> GetImageSemaphores() const
		{
			return _image_semaphores;
		}

		const container::vector<VkSemaphore> GetRenderSemaphores() const
		{
			return _render_semaphores;
		}

		const container::vector<VkFence> GetFences() const
		{
			return _fences;
		}

		const container::vector<VkFence> GetImageFences() const
		{
			return _acquired_image_fences;
		}

		operator VkSwapchainKHR() const
		{
			return _swapchain;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SWAPCHAIN_HPP */