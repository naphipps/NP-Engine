//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_IMAGE_VIEW_HPP
#define NP_ENGINE_VULKAN_IMAGE_VIEW_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanDevice.hpp"

namespace np::gfx::__detail
{
	class VulkanImageView
	{
	private:
		VulkanDevice& _device;
		VkImageView _image_view;

		VkImageView CreateImageView(VkImageViewCreateInfo& info)
		{
			VkImageView image_view = nullptr;

			if (vkCreateImageView(GetDevice(), &info, nullptr, &image_view) != VK_SUCCESS)
			{
				image_view = nullptr;
			}

			return image_view;
		}

	public:
		static VkImageViewCreateInfo CreateInfo()
		{
			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			info.format = VK_FORMAT_R8G8B8A8_SRGB;
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

		VulkanImageView(VulkanDevice& device, VkImageViewCreateInfo& image_view_create_info):
			_device(device),
			_image_view(CreateImageView(image_view_create_info))
		{}

		VulkanImageView(const VulkanImageView&) = delete;

		VulkanImageView(VulkanImageView&& other) noexcept: _device(other._device), _image_view(::std::move(other._image_view))
		{
			other._image_view = nullptr;
		}

		~VulkanImageView()
		{
			if (_image_view)
			{
				vkDestroyImageView(GetDevice(), _image_view, nullptr);
				_image_view = nullptr;
			}
		}

		operator VkImageView() const
		{
			return _image_view;
		}

		VulkanDevice& GetDevice() const
		{
			return _device;
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_IMAGE_VIEW_HPP */