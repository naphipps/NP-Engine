//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_IMAGE_RESOURCE_VIEW_HPP
#define NP_ENGINE_GPU_VULKAN_IMAGE_RESOURCE_VIEW_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanDevice.hpp"
#include "VulkanImageResource.hpp"

namespace np::gpu::__detail
{
	class VulkanImageResourceView : public ImageResourceView
	{
	private:
		mem::sptr<VulkanDevice> _device;
		mem::sptr<VulkanImageResource> _image;
		VkImageView _view;

		static VkImageViewCreateInfo CreateVkInfo()
		{
			VkImageViewCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY; //TODO: investigate if we want/need to allow caller to set these
			info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			return info;
		}

		static VkImageView CreateVkImageView(mem::sptr<VulkanDevice> device, mem::sptr<VulkanImageResource> image, VulkanImageResourceUsage usage)
		{
			const VulkanFormat format{image->GetFormat()};

			VkImageViewCreateInfo info = CreateVkInfo();
			info.image = *image;
			info.format = format.GetVkFormat();
			info.viewType = image->GetVkImageViewType();
			info.subresourceRange.levelCount = image->GetMipCount();
			info.subresourceRange.layerCount = image->GetLayerCount();
			info.subresourceRange.aspectMask = usage.GetVkAspectFlags(); //TODO: investigate -- what is this?
			
			VkImageView view = nullptr;
			VkResult result = vkCreateImageView(*device->GetLogicalDevice(), &info, nullptr, &view);
			return result == VK_SUCCESS ? view : nullptr;
		}

	public:
		VulkanImageResourceView(mem::sptr<Device> device, mem::sptr<ImageResource> image, ImageResourceUsage usage):
			_device(device),
			_image(image),
			_view(CreateVkImageView(_device, _image, VulkanImageResourceUsage{ usage }))
		{}

		~VulkanImageResourceView()
		{
			if (_view)
			{
				vkDestroyImageView(*_device->GetLogicalDevice(), _view, nullptr);
				_view = nullptr;
			}
		}

		operator VkImageView() const
		{
			return _view;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _device;
		}

		virtual mem::sptr<ImageResource> GetImageResource() const override
		{
			return _image;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_IMAGE_RESOURCE_VIEW_HPP */