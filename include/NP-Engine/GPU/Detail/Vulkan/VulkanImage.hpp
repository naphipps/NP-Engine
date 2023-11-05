//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_IMAGE_HPP
#define NP_ENGINE_VULKAN_IMAGE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanImage
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkImage _image;
		VkDeviceMemory _device_memory;

		static VkImage CreateImage(mem::sptr<VulkanLogicalDevice> device, const VkImageCreateInfo& info)
		{
			VkImage image = nullptr;
			if (vkCreateImage(*device, &info, nullptr, &image) != VK_SUCCESS)
				image = nullptr;

			return image;
		}

		static VkDeviceMemory CreateDeviceMemory(mem::sptr<VulkanLogicalDevice> device, VkImage image,
												 VkMemoryPropertyFlags memory_property_flags)
		{
			VkDeviceMemory device_memory = nullptr;

			if (image)
			{
				VkMemoryRequirements requirements;
				vkGetImageMemoryRequirements(*device, image, &requirements);

				VkMemoryAllocateInfo allocate_info{};
				allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocate_info.allocationSize = requirements.size;
				allocate_info.memoryTypeIndex =
					device->GetMemoryTypeIndex(requirements.memoryTypeBits, memory_property_flags).value();

				if (vkAllocateMemory(*device, &allocate_info, nullptr, &device_memory) != VK_SUCCESS)
					device_memory = nullptr;
				else if (vkBindImageMemory(*device, image, device_memory, 0) != VK_SUCCESS)
					device_memory = nullptr;
			}

			return device_memory;
		}

	public:
		static VkImageCreateInfo CreateInfo()
		{
			VkImageCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.imageType = VK_IMAGE_TYPE_2D;
			info.extent = {0, 0, 1};
			info.format = VK_FORMAT_R8G8B8A8_SRGB;
			info.tiling = VK_IMAGE_TILING_OPTIMAL;
			info.mipLevels = 1;
			info.arrayLayers = 1;
			info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			info.samples = VK_SAMPLE_COUNT_1_BIT;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			return info;
		}

		VulkanImage(mem::sptr<VulkanLogicalDevice> device, const VkImageCreateInfo& image_create_info,
					VkMemoryPropertyFlags memory_property_flags):
			_device(device),
			_image(CreateImage(_device, image_create_info)),
			_device_memory(CreateDeviceMemory(_device, _image, memory_property_flags))
		{}

		VulkanImage(VulkanImage&& other) noexcept:
			_device(::std::move(other._device)),
			_image(::std::move(other._image)),
			_device_memory(::std::move(other._device_memory))
		{
			other._device.reset();
			other._image = nullptr;
			other._device_memory = nullptr;
		}

		~VulkanImage()
		{
			vkDeviceWaitIdle(*_device);

			if (_image)
			{
				vkDestroyImage(*_device, _image, nullptr);
				_image = nullptr;
			}

			if (_device_memory)
			{
				vkFreeMemory(*_device, _device_memory, nullptr);
				_device_memory = nullptr;
			}
		}

		operator VkImage() const
		{
			return _image;
		}

		VkDeviceMemory GetDeviceMemory() const
		{
			return _device_memory;
		}

		mem::sptr<VulkanLogicalDevice> GetDevice() const
		{
			return _device;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_IMAGE_HPP */