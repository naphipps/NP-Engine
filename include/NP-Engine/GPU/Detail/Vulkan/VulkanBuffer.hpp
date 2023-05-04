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
#include "VulkanCommands.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanQueue.hpp"
#include "VulkanFence.hpp"

namespace np::gpu::__detail
{
	class VulkanBuffer : public Buffer
	{
	private:
		mem::sptr<VulkanCommandPool> _command_pool;
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

		static VkDeviceMemory CreateDeviceMemory(mem::sptr<VulkanLogicalDevice> device, VkBuffer buffer, VkMemoryPropertyFlags memory_property_flags)
		{
			VkDeviceMemory device_memory = nullptr;

			VkMemoryRequirements requirements;
			vkGetBufferMemoryRequirements(*device, buffer, &requirements);

			VkMemoryAllocateInfo allocate_info{};
			allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocate_info.allocationSize = requirements.size;
			allocate_info.memoryTypeIndex = device->GetMemoryTypeIndex(requirements.memoryTypeBits, memory_property_flags).value();

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

		VulkanBuffer(mem::sptr<VulkanCommandPool> command_pool, VkBufferCreateInfo& buffer_create_info, VkMemoryPropertyFlags memory_property_flags):
			Buffer(),
			_command_pool(command_pool),
			_buffer(CreateBuffer(_command_pool->GetLogicalDevice(), buffer_create_info)),
			_device_memory(CreateDeviceMemory(_command_pool->GetLogicalDevice(), _buffer, memory_property_flags)),
			_size(buffer_create_info.size)
		{}

		VulkanBuffer(VulkanBuffer&& other) noexcept:
			_command_pool(::std::move(other._command_pool)),
			_buffer(::std::move(other._buffer)),
			_device_memory(::std::move(other._device_memory)),
			_size(::std::move(other._size))
		{
			other._command_pool = nullptr;
			other._buffer = nullptr;
			other._device_memory = nullptr;
			other._size = 0;
		}

		~VulkanBuffer()
		{
			if (_buffer)
			{
				vkDestroyBuffer(*GetLogicalDevice(), _buffer, nullptr);
				_buffer = nullptr;
			}

			if (_device_memory)
			{
				vkFreeMemory(*GetLogicalDevice(), _device_memory, nullptr);
				_device_memory = nullptr;
			}
		}

		operator VkBuffer() const
		{
			return _buffer;
		}

		VkResult AsyncCopyTo(VulkanBuffer& other, VkSubmitInfo& submit_info, con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers, mem::sptr<VulkanQueue> queue, VkFence fence = nullptr)
		{
			VkBufferCopy buffer_copy{};
			buffer_copy.size = _size;
			VulkanCommandCopyBuffers copy_buffers(_buffer, other, 1, &buffer_copy);

			con::vector<mem::sptr<VulkanCommandBuffer>> buffers = _command_pool->AllocateCommandBuffers(1);
			VkCommandBufferBeginInfo begin_info = VulkanCommandBuffer::CreateBeginInfo();
			begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			VulkanCommandPool::BeginCommandBuffers(buffers, begin_info);
			buffers.front()->Add(copy_buffers);
			VulkanCommandPool::EndCommandBuffers(buffers);
			for (auto it = buffers.begin(); it != buffers.end(); it++)
				command_buffers.emplace_back(*it);
			return queue->Submit(buffers, submit_info, fence);
		}

		VkResult SyncCopyTo(VulkanBuffer& other, VkSubmitInfo& submit_info, mem::sptr<VulkanQueue> queue)
		{
			con::vector<mem::sptr<VulkanCommandBuffer>> command_buffers;
			VulkanFence fence(GetLogicalDevice());
			VkResult result = AsyncCopyTo(other, submit_info, command_buffers, queue, fence);
			fence.Wait();
			_command_pool->FreeCommandBuffers(command_buffers);
			return result;
		}

		void AssignData(const void* data)
		{
			void* mapping = nullptr;
			if (vkMapMemory(*GetLogicalDevice(), GetDeviceMemory(), 0, GetSize(), 0, &mapping) != VK_SUCCESS)
				mapping = nullptr;
			if (mapping)
				memcpy(mapping, data, GetSize());
			vkUnmapMemory(*GetLogicalDevice(), GetDeviceMemory());
		}

		mem::sptr<VulkanCommandPool> GetCommandPool() const
		{
			return _command_pool;
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _command_pool->GetLogicalDevice();
		}

		VkDeviceMemory GetDeviceMemory() const
		{
			return _device_memory;
		}

		VkDeviceSize GetSize() const
		{
			return _size;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_BUFFER_HPP */