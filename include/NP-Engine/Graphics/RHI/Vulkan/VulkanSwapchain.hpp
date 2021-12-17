//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SWAPCHAIN_HPP
#define NP_ENGINE_VULKAN_SWAPCHAIN_HPP

#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"

namespace np::graphics::rhi
{
	class VulkanSwapchain
	{
	private:
		VulkanDevice& _device;
		VkExtent2D _extent;
		VkSwapchainKHR _swapchain;
		container::vector<VkImage> _images;
		container::vector<VkImageView> _image_views;

		VkSwapchainCreateInfoKHR CreateSwapchainInfo()
		{
			VkSwapchainCreateInfoKHR info{};
			info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			info.surface = Surface();
			info.imageFormat = Device().SurfaceFormat().format;
			info.imageColorSpace = Device().SurfaceFormat().colorSpace;
			info.imageExtent = _extent;
			info.imageArrayLayers = 1;
			info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			return info;
		}

		VkImageViewCreateInfo CreateImageViewInfo()
		{
			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = Device().SurfaceFormat().format;
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

		VkExtent2D ChooseExtent()
		{
			VkExtent2D extent{};

			if (Device().Capabilities().currentExtent.width != UI32_MAX)
			{
				extent = Device().Capabilities().currentExtent;
			}
			else
			{
				i32 width, height;
				glfwGetFramebufferSize((GLFWwindow*)Surface().Window().GetNativeWindow(), &width, &height);

				VkExtent2D min_extent = Device().Capabilities().minImageExtent;
				VkExtent2D max_extent = Device().Capabilities().maxImageExtent;

				extent = {::std::clamp((ui32)width, min_extent.width, max_extent.width),
						  ::std::clamp((ui32)height, min_extent.height, max_extent.height)};
			}

			return extent;
		}

		VkSwapchainKHR CreateSwapchain()
		{
			VkSwapchainKHR swapchain = nullptr;

			ui32 min_image_count = Device().Capabilities().minImageCount + 1;
			if (Device().Capabilities().maxImageCount != 0)
				min_image_count = ::std::min(min_image_count, Device().Capabilities().maxImageCount);

			container::vector<ui32> indices = Device().QueueFamilyIndices().to_vector();

			VkSwapchainCreateInfoKHR swapchain_info = CreateSwapchainInfo();
			swapchain_info.minImageCount = min_image_count;

			if (Device().QueueFamilyIndices().graphics != Device().QueueFamilyIndices().present)
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
			swapchain_info.preTransform = Device().Capabilities().currentTransform;
			swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			swapchain_info.presentMode = Device().PresentMode();
			swapchain_info.clipped = VK_TRUE;
			swapchain_info.oldSwapchain = nullptr;

			if (vkCreateSwapchainKHR(Device(), &swapchain_info, nullptr, &swapchain) != VK_SUCCESS)
			{
				swapchain = nullptr;
			}

			return swapchain;
		}

		container::vector<VkImage> RetrieveImages()
		{
			container::vector<VkImage> images;

			if (_swapchain != nullptr)
			{
				ui32 count;
				vkGetSwapchainImagesKHR(Device(), _swapchain, &count, nullptr);
				images.resize(count);
				vkGetSwapchainImagesKHR(Device(), _swapchain, &count, images.data());
			}

			return images;
		}

		container::vector<VkImageView> CreateImageViews()
		{
			container::vector<VkImageView> views(_images.size());

			for (siz i = 0; i < _images.size(); i++)
			{
				VkImageViewCreateInfo image_view_info = CreateImageViewInfo();
				image_view_info.image = _images[i];
				if (vkCreateImageView(Device(), &image_view_info, nullptr, &views[i]) != VK_SUCCESS)
				{
					views.clear();
					break;
				}
			}

			return views;
		}

		void Dispose()
		{
			for (VkImageView& view : _image_views)
				vkDestroyImageView(Device(), view, nullptr);

			if (_swapchain != nullptr)
				vkDestroySwapchainKHR(Device(), _swapchain, nullptr);
		}

	public:
		VulkanSwapchain(VulkanDevice& device):
			_device(device),
			_extent(ChooseExtent()),
			_swapchain(CreateSwapchain()),
			_images(RetrieveImages()),
			_image_views(CreateImageViews())
		{}

		~VulkanSwapchain()
		{
			Dispose();
		}

		VulkanInstance& Instance() const
		{
			return _device.Instance();
		}

		VulkanSurface& Surface() const
		{
			return _device.Surface();
		}

		VulkanDevice& Device() const
		{
			return _device;
		}

		void Rebuild()
		{
			Dispose();
			_extent = ChooseExtent();
			_swapchain = CreateSwapchain();
			_images = RetrieveImages();
			_image_views = CreateImageViews();
		}

		const container::vector<VkImage>& Images() const
		{
			return _images;
		}

		const container::vector<VkImageView>& ImageViews() const
		{
			return _image_views;
		}

		const VkExtent2D& Extent() const
		{
			return _extent;
		}

		operator VkSwapchainKHR() const
		{
			return _swapchain;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SWAPCHAIN_HPP */