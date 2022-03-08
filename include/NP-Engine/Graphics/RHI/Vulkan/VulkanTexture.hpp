//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_TEXTURE_HPP
#define NP_ENGINE_VULKAN_TEXTURE_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanSampler.hpp"

namespace np::graphics::rhi
{
	class VulkanTexture : public Texture
	{
	public:
		struct CreateInfoType
		{
			VkImageCreateInfo ImageCreateInfo{};
			VkMemoryPropertyFlags ImageMemoryPropertyFlags = 0;
			VkImageViewCreateInfo ImageViewCreateInfo{};
		};

	private:
		VulkanDevice& _device;
		VulkanImage _image;
		VulkanImageView _image_view;

		VkImageViewCreateInfo& ApplyImageToImageViewCreateInfo(VkImageViewCreateInfo& info)
		{
			info.image = _image;
			return info;
		}

	public:
		VulkanTexture(VulkanDevice& device, VkImageCreateInfo& image_create_info, VkMemoryPropertyFlags memory_property_flags,
					  VkImageViewCreateInfo& image_view_create_info):
			_device(device),
			_image(device, image_create_info, memory_property_flags),
			_image_view(device, ApplyImageToImageViewCreateInfo(image_view_create_info))
		{}

		VulkanTexture(const VulkanTexture&) = delete;

		VulkanTexture(VulkanTexture&& other) noexcept:
			_device(::std::move(other._device)),
			_image(::std::move(other._image)),
			_image_view(::std::move(other._image_view))
		{}

		~VulkanTexture() = default;

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
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_TEXTURE_HPP */