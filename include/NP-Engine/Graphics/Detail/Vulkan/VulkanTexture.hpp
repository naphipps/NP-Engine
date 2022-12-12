//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_TEXTURE_HPP
#define NP_ENGINE_VULKAN_TEXTURE_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/Interface/Interface.hpp"

#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanSampler.hpp"

namespace np::gfx::__detail
{
	class VulkanTexture
	{
	public:
		struct CreateInfoType // TODO: can we remove this??
		{
			VkImageCreateInfo ImageCreateInfo{};
			VkMemoryPropertyFlags ImageMemoryPropertyFlags = 0;
			VkImageViewCreateInfo ImageViewCreateInfo{};
		};

	private:
		VulkanDevice& _device;
		VulkanImage _image;
		VulkanImageView _image_view;
		bl _hot_reloadable;
		ui32 _width;
		ui32 _height;

		VkImageViewCreateInfo& ApplyImageToImageViewCreateInfo(VkImageViewCreateInfo& info)
		{
			info.image = _image;
			return info;
		}

	public:
		VulkanTexture(VulkanDevice& device, VkImageCreateInfo& image_create_info, VkMemoryPropertyFlags memory_property_flags,
					  VkImageViewCreateInfo& image_view_create_info, bl hot_reloadable = false):
			_device(device),
			_image(device, image_create_info, memory_property_flags),
			_image_view(device, ApplyImageToImageViewCreateInfo(image_view_create_info)),
			_hot_reloadable(hot_reloadable),
			_width(image_create_info.extent.width),
			_height(image_create_info.extent.height)
		{}

		VulkanTexture(const VulkanTexture&) = delete;

		VulkanTexture(VulkanTexture&& other) noexcept:
			_device(other._device),
			_image(::std::move(other._image)),
			_image_view(::std::move(other._image_view)),
			_hot_reloadable(::std::move(other._hot_reloadable)),
			_width(::std::move(other._width)),
			_height(::std::move(other._height))
		{}

		VulkanTexture& operator=(const VulkanTexture& other) = delete;

		// TODO: can we add move operator??

		VulkanDevice& GetDevice() const
		{
			return _device;
		}

		VulkanImage& GetImage()
		{
			return _image;
		}

		const VulkanImage& GetImage() const
		{
			return _image;
		}

		VulkanImageView& GetImageView()
		{
			return _image_view;
		}

		const VulkanImageView& GetImageView() const
		{
			return _image_view;
		}

		void SetHotReloadable(bl hot_reloadable = true)
		{
			_hot_reloadable = hot_reloadable;
		}

		bl IsHotReloadable() const
		{
			return _hot_reloadable;
		}

		ui32 GetWidth() const
		{
			return _width;
		}

		ui32 GetHeight() const
		{
			return _height;
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_TEXTURE_HPP */