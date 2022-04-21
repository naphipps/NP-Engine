//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/20/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_BUFFER_HPP
#define NP_ENGINE_VULKAN_BUFFER_HPP

#include <iostream> //TODO: remove
#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanDevice.hpp"

namespace np::graphics::rhi
{
	class VulkanBuffer : public Buffer
	{
	private:
		VulkanDevice& _device;
		VkBuffer _buffer;
		VkDeviceMemory _device_memory;
		VkDeviceSize _size; // TODO: do we need this??

		VkBuffer CreateBuffer(VkBufferCreateInfo& info)
		{
			VkBuffer buffer = nullptr;

			if (vkCreateBuffer(GetDevice(), &info, nullptr, &buffer) != VK_SUCCESS)
			{
				buffer = nullptr;
			}

			return buffer;
		}

		VkDeviceMemory CreateDeviceMemory(VkMemoryPropertyFlags memory_property_flags)
		{
			VkDeviceMemory device_memory = nullptr;

			VkMemoryRequirements requirements;
			vkGetBufferMemoryRequirements(GetDevice(), _buffer, &requirements);

			VkMemoryAllocateInfo allocate_info{};
			allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocate_info.allocationSize = requirements.size;
			allocate_info.memoryTypeIndex = GetDevice().GetMemoryTypeIndex(requirements.memoryTypeBits, memory_property_flags);

			if (vkAllocateMemory(GetDevice(), &allocate_info, nullptr, &device_memory) != VK_SUCCESS)
			{
				device_memory = nullptr;
			}
			else
			{
				vkBindBufferMemory(GetDevice(), _buffer, device_memory, 0);
			}

			return device_memory;
		}

	public:
		static void Copy(VulkanBuffer& dst, VulkanBuffer& src,
						 siz size) // TODO: I think we should opt for a CopyTo or CopyFrom method instead
		{
			// TODO: figure out a way to include fence/semaphore parameters/returns/etc

			VkBufferCopy buffer_copy{};
			buffer_copy.size = size;
			VulkanCommandCopyBuffers copy_buffers(src, dst, 1, &buffer_copy);

			container::vector<VulkanCommandBuffer> command_buffers = dst.GetDevice().BeginSingleUseCommandBuffers(1);
			command_buffers.front().Add(copy_buffers);
			dst.GetDevice().EndSingleUseCommandBuffers(command_buffers);
		}

		static VkBufferCreateInfo CreateInfo()
		{
			VkBufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			return info;
		}

		VulkanBuffer(VulkanDevice& device, VkBufferCreateInfo& buffer_create_info, VkMemoryPropertyFlags memory_property_flags):
			Buffer(),
			_device(device),
			_buffer(CreateBuffer(buffer_create_info)),
			_device_memory(CreateDeviceMemory(memory_property_flags))
		{}

		VulkanBuffer(const VulkanBuffer&) = delete;

		VulkanBuffer(VulkanBuffer&& other) noexcept:
			_device(other._device),
			_buffer(::std::move(other._buffer)),
			_device_memory(::std::move(other._device_memory)),
			_size(::std::move(other._size))
		{
			other._buffer = nullptr;
			other._device_memory = nullptr;
			other._size = 0;
		}

		~VulkanBuffer()
		{
			if (_buffer)
				vkDestroyBuffer(GetDevice(), _buffer, nullptr);

			if (_device_memory)
				vkFreeMemory(GetDevice(), _device_memory, nullptr);
		}

		VulkanInstance& GetInstance() const // TODO: the following methods should be added to every Vulkan class possible
		{
			return _device.GetInstance();
		}

		VulkanSurface& GetSurface() const
		{
			return _device.GetSurface();
		}

		VulkanDevice& GetDevice() const
		{
			return _device;
		}

		VkDeviceMemory GetDeviceMemory() const
		{
			return _device_memory;
		}

		operator VkBuffer() const
		{
			return _buffer;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_BUFFER_HPP */