//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_TEXTURE_HPP
#define NP_ENGINE_VULKAN_TEXTURE_HPP

#include <utility>

#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/Interface/Interface.hpp"

#include "VulkanCommandPool.hpp"
#include "VulkanLogicalDevice.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanSampler.hpp"

namespace np::gpu::__detail
{
	class VulkanTexture
	{
	private:
		mem::sptr<VulkanCommandPool> _command_pool;
		VulkanImage _image;
		VulkanImageView _image_view;
		bl _hot_reloadable;
		ui32 _width;
		ui32 _height;

		static VkImageViewCreateInfo& ApplyImageToImageViewCreateInfo(VkImage image, VkImageViewCreateInfo& info)
		{
			info.image = image;
			return info;
		}

	public:
		VulkanTexture(mem::sptr<VulkanCommandPool> command_pool, 
			VkImageCreateInfo& image_create_info, 
			VkMemoryPropertyFlags memory_property_flags,
			VkImageViewCreateInfo& image_view_create_info,
			bl hot_reloadable = false):
			_command_pool(command_pool),
			_image(GetCommandPool(), image_create_info, memory_property_flags),
			_image_view(GetLogicalDevice(), ApplyImageToImageViewCreateInfo(_image, image_view_create_info)),
			_hot_reloadable(hot_reloadable),
			_width(image_create_info.extent.width),
			_height(image_create_info.extent.height)
		{}

		VulkanTexture(VulkanTexture&& other) noexcept:
			_command_pool(::std::move(other._command_pool)),
			_image(::std::move(other._image)),
			_image_view(::std::move(other._image_view)),
			_hot_reloadable(::std::move(other._hot_reloadable)),
			_width(::std::move(other._width)),
			_height(::std::move(other._height))
		{}

		mem::sptr<VulkanCommandPool> GetCommandPool() const
		{
			return _command_pool;
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _command_pool->GetLogicalDevice();
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
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_TEXTURE_HPP */