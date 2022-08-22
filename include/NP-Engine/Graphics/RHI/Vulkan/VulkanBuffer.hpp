//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/20/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_BUFFER_HPP
#define NP_ENGINE_VULKAN_BUFFER_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanDevice.hpp"
#include "VulkanCommands.hpp"
#include "VulkanFence.hpp"

namespace np::gfx::rhi
{
	class VulkanBuffer : public Buffer
	{
	private:
		VulkanDevice& _device;
		VkBuffer _buffer;
		VkDeviceMemory _device_memory;
		VkDeviceSize _size;

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
			_device_memory(CreateDeviceMemory(memory_property_flags)),
			_size(buffer_create_info.size)
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
			{
				vkDestroyBuffer(GetDevice(), _buffer, nullptr);
				_buffer = nullptr;
			}

			if (_device_memory)
			{
				vkFreeMemory(GetDevice(), _device_memory, nullptr);
				_device_memory = nullptr;
			}
		}

		VkResult AsyncCopyTo(VulkanBuffer& other, VkSubmitInfo& submit_info,
							 con::vector<VulkanCommandBuffer>& command_buffers, VkFence fence = nullptr)
		{
			VkBufferCopy buffer_copy{};
			buffer_copy.size = _size;
			VulkanCommandCopyBuffers copy_buffers(_buffer, other, 1, &buffer_copy);

			// TODO: how are we freeing these command_buffers?
			con::vector<VulkanCommandBuffer> buffers = GetDevice().AllocateCommandBuffers(1);
			VkCommandBufferBeginInfo begin_info = VulkanCommandBuffer::CreateBeginInfo();
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			GetDevice().BeginCommandBuffers(buffers, begin_info);
			buffers.front().Add(copy_buffers);
			GetDevice().EndCommandBuffers(buffers);
			for (auto it = buffers.begin(); it != buffers.end(); it++)
				command_buffers.emplace_back(*it);
			return GetDevice().GetGraphicsQueue().Submit(buffers, submit_info, fence);
		}

		VkResult SyncCopyTo(VulkanBuffer& other, VkSubmitInfo& submit_info)
		{
			con::vector<VulkanCommandBuffer> command_buffers;
			VulkanFence fence(GetDevice());
			VkResult result = AsyncCopyTo(other, submit_info, command_buffers, fence);
			fence.Wait();
			GetDevice().FreeCommandBuffers(command_buffers);
			return result;
		}

		void AssignData(const void* data)
		{
			void* mapping = nullptr;
			if (vkMapMemory(GetDevice(), GetDeviceMemory(), 0, GetSize(), 0, &mapping) != VK_SUCCESS)
				mapping = nullptr;
			if (mapping)
				memcpy(mapping, data, GetSize());
			vkUnmapMemory(GetDevice(), GetDeviceMemory());
		}

		// TODO: the following methods should be added to every Vulkan class possible

		VulkanInstance& GetInstance()
		{
			return _device.GetInstance();
		}

		const VulkanInstance& GetInstance() const
		{
			return _device.GetInstance();
		}

		VulkanSurface& GetSurface()
		{
			return _device.GetSurface();
		}

		const VulkanSurface& GetSurface() const
		{
			return _device.GetSurface();
		}

		VulkanDevice& GetDevice()
		{
			return _device;
		}

		const VulkanDevice& GetDevice() const
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

		operator VkBuffer() const
		{
			return _buffer;
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_BUFFER_HPP */