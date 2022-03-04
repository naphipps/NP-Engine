//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/2/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_POOL_HPP
#define NP_ENGINE_VULKAN_COMMAND_POOL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanDevice.hpp"
#include "VulkanCommandBuffer.hpp"

namespace np::graphics::rhi
{
	class VulkanCommandPool
	{
	private:
		VulkanDevice& _device;
		VkCommandPool _command_pool;

		VkCommandPoolCreateInfo CreateCommandPoolInfo()
		{
			VkCommandPoolCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.queueFamilyIndex = GetDevice().GetQueueFamilyIndices().graphics.value();
			return info;
		}

		VkCommandPool CreateCommandPool(VkCommandPoolCreateFlags command_pool_create_flags)
		{
			VkCommandPool command_pool = nullptr;
			VkCommandPoolCreateInfo command_pool_info = CreateCommandPoolInfo();
			command_pool_info.flags = command_pool_create_flags;
			if (vkCreateCommandPool(GetDevice(), &command_pool_info, nullptr, &command_pool) != VK_SUCCESS)
			{
				command_pool = nullptr;
			}
			return command_pool;
		}

	public:
		VulkanCommandPool(VulkanDevice& device, VkCommandPoolCreateFlags command_pool_create_flags = 0):
			_device(device),
			_command_pool(CreateCommandPool(command_pool_create_flags))
		{}

		~VulkanCommandPool()
		{
			vkDestroyCommandPool(GetDevice(), _command_pool, nullptr);
		}

		VulkanDevice& GetDevice() const
		{
			return _device;
		}

		operator VkCommandPool() const
		{
			return _command_pool;
		}

		container::vector<VulkanCommandBuffer> AllocateCommandBuffers(
			const VkCommandBufferAllocateInfo& command_buffer_allocate_info)
		{
			container::vector<VkCommandBuffer> buffers(command_buffer_allocate_info.commandBufferCount);
			container::vector<VulkanCommandBuffer> command_buffers;

			if (vkAllocateCommandBuffers(GetDevice(), &command_buffer_allocate_info, buffers.data()) != VK_SUCCESS)
			{
				buffers.clear();
			}

			command_buffers.insert(command_buffers.begin(), buffers.begin(), buffers.end());

			return command_buffers;
		}

		void FreeCommandBuffers(const container::vector<VulkanCommandBuffer>& command_buffers)
		{
			container::vector<VkCommandBuffer> buffers(command_buffers.begin(), command_buffers.end());
			vkFreeCommandBuffers(GetDevice(), _command_pool, (ui32)buffers.size(), buffers.data());
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_POOL_HPP */