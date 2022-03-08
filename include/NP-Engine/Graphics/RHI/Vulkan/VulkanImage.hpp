//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/1/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_IMAGE_HPP
#define NP_ENGINE_VULKAN_IMAGE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanDevice.hpp"

namespace np::graphics::rhi
{
	class VulkanImage
	{
	private:
		VulkanDevice& _device;
		VkImage _image;
		VkDeviceMemory _device_memory;

		VkImage CreateImage(VkImageCreateInfo& info)
		{
			VkImage image = nullptr;

			if (vkCreateImage(GetDevice(), &info, nullptr, &image) != VK_SUCCESS)
			{
				image = nullptr;
			}

			return image;
		}

		VkDeviceMemory CreateDeviceMemory(VkMemoryPropertyFlags memory_property_flags)
		{
			VkDeviceMemory device_memory = nullptr;

			if (_image)
			{
				VkMemoryRequirements requirements;
				vkGetImageMemoryRequirements(GetDevice(), _image, &requirements);

				VkMemoryAllocateInfo allocate_info{};
				allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
				allocate_info.allocationSize = requirements.size;
				allocate_info.memoryTypeIndex =
					GetDevice().GetMemoryTypeIndex(requirements.memoryTypeBits, memory_property_flags);

				if (vkAllocateMemory(GetDevice(), &allocate_info, nullptr, &device_memory) != VK_SUCCESS)
				{
					device_memory = nullptr;
				}
				else
				{
					vkBindImageMemory(GetDevice(), _image, device_memory, 0);
				}
			}

			return device_memory;
		}

	public:
		static VkImageCreateInfo CreateInfo()
		{
			VkImageCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			info.imageType = VK_IMAGE_TYPE_2D; // TODO: feel like this could be a constructor param??
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

		VulkanImage(VulkanDevice& device, VkImageCreateInfo& image_create_info, VkMemoryPropertyFlags memory_property_flags):
			_device(device),
			_image(CreateImage(image_create_info)),
			_device_memory(CreateDeviceMemory(memory_property_flags))
		{}

		// TODO: add move constructor?

		VulkanImage(const VulkanImage&) = delete;

		VulkanImage(VulkanImage&& other) noexcept:
			_device(::std::move(other._device)),
			_image(::std::move(other._image)),
			_device_memory(::std::move(other._device_memory))
		{
			other._image = nullptr;
			other._device_memory = nullptr;
		}

		~VulkanImage()
		{
			if (_image)
				vkDestroyImage(GetDevice(), _image, nullptr);

			if (_device_memory)
				vkFreeMemory(GetDevice(), _device_memory, nullptr);
		}

		operator VkImage() const
		{
			return _image;
		}

		VkDeviceMemory GetDeviceMemory() const
		{
			return _device_memory;
		}

		// TODO: GetWidth and GetHeight??

		VulkanDevice& GetDevice() const
		{
			return _device;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_IMAGE_HPP */