//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/20/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_BUFFER_HPP
#define NP_ENGINE_VULKAN_BUFFER_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanBuffer : public Buffer
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkBuffer _buffer;
		VkDeviceMemory _device_memory;
		VkDeviceSize _size;

		static VkBuffer CreateBuffer(mem::sptr<VulkanLogicalDevice> device, VkBufferCreateInfo& info)
		{
			VkBuffer buffer = nullptr;
			if (vkCreateBuffer(*device, &info, nullptr, &buffer) != VK_SUCCESS)
				buffer = nullptr;

			return buffer;
		}

		static VkDeviceMemory CreateDeviceMemory(mem::sptr<VulkanLogicalDevice> device, VkBuffer buffer,
												 VkMemoryPropertyFlags memory_property_flags)
		{
			VkDeviceMemory device_memory = nullptr;

			VkMemoryRequirements requirements;
			vkGetBufferMemoryRequirements(*device, buffer, &requirements);

			VkMemoryAllocateInfo allocate_info{};
			allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocate_info.allocationSize = requirements.size;
			allocate_info.memoryTypeIndex =
				device->GetMemoryTypeIndex(requirements.memoryTypeBits, memory_property_flags).value();

			if (vkAllocateMemory(*device, &allocate_info, nullptr, &device_memory) != VK_SUCCESS)
				device_memory = nullptr;
			else
				vkBindBufferMemory(*device, buffer, device_memory, 0);

			return device_memory;
		}

	public:
		static VkBufferCreateInfo CreateInfo()
		{
			VkBufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			return info;
		}

		VulkanBuffer(mem::sptr<VulkanLogicalDevice> device, VkBufferCreateInfo& buffer_create_info,
					 VkMemoryPropertyFlags memory_property_flags):
			Buffer(),
			_device(device),
			_buffer(CreateBuffer(_device, buffer_create_info)),
			_device_memory(CreateDeviceMemory(_device, _buffer, memory_property_flags)),
			_size(buffer_create_info.size)
		{}

		VulkanBuffer(VulkanBuffer&& other) noexcept:
			_device(::std::move(other._device)),
			_buffer(::std::move(other._buffer)),
			_device_memory(::std::move(other._device_memory)),
			_size(::std::move(other._size))
		{
			other._device.reset();
			other._buffer = nullptr;
			other._device_memory = nullptr;
			other._size = 0;
		}

		~VulkanBuffer()
		{
			vkDeviceWaitIdle(*_device);

			if (_buffer)
			{
				vkDestroyBuffer(*_device, _buffer, nullptr);
				_buffer = nullptr;
			}

			if (_device_memory)
			{
				vkFreeMemory(*_device, _device_memory, nullptr);
				_device_memory = nullptr;
			}
		}

		operator VkBuffer() const
		{
			return _buffer;
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _device;
		}

		VkDeviceMemory GetDeviceMemory() const
		{
			return _device_memory;
		}

		VkDeviceSize GetSize() const
		{
			return _size;
		}

		void AssignData(const void* data)
		{
			void* mapping = nullptr;
			if (vkMapMemory(*_device, GetDeviceMemory(), 0, GetSize(), 0, &mapping) != VK_SUCCESS)
				mapping = nullptr;
			if (mapping)
				memcpy(mapping, data, GetSize());
			vkUnmapMemory(*_device, GetDeviceMemory());
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_BUFFER_HPP */