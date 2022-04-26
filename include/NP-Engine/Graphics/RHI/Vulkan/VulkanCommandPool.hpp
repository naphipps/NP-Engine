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

#include "VulkanCommandBuffer.hpp"

namespace np::graphics::rhi
{
	class VulkanCommandPool
	{
	private:
		VkDevice& _device;
		VkCommandPool _command_pool;

		VkCommandPool CreateCommandPool(VkCommandPoolCreateInfo& info) const
		{
			VkCommandPool command_pool = nullptr;
			if (vkCreateCommandPool(_device, &info, nullptr, &command_pool) != VK_SUCCESS)
			{
				command_pool = nullptr;
			}
			return command_pool;
		}

	public:
		static VkCommandPoolCreateInfo CreateInfo()
		{
			VkCommandPoolCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			return info;
		}

		VulkanCommandPool(VkDevice& device, VkCommandPoolCreateInfo command_pool_create_info):
			_device(device),
			_command_pool(CreateCommandPool(command_pool_create_info))
		{}

		~VulkanCommandPool()
		{
			if (_command_pool)
			{
				vkDestroyCommandPool(_device, _command_pool, nullptr);
				_command_pool = nullptr;
			}
		}

		operator VkCommandPool() const
		{
			return _command_pool;
		}

		VkCommandBufferAllocateInfo CreateCommandBufferAllocateInfo() const
		{
			VkCommandBufferAllocateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.commandPool = _command_pool;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			return info;
		}

		container::vector<VulkanCommandBuffer> AllocateCommandBuffers(
			const VkCommandBufferAllocateInfo& command_buffer_allocate_info)
		{
			container::vector<VkCommandBuffer> buffers(command_buffer_allocate_info.commandBufferCount);

			if (vkAllocateCommandBuffers(_device, &command_buffer_allocate_info, buffers.data()) != VK_SUCCESS)
			{
				buffers.clear();
			}

			return container::vector<VulkanCommandBuffer>(buffers.begin(), buffers.end());
		}

		void FreeCommandBuffers(const container::vector<VulkanCommandBuffer>& command_buffers)
		{
			container::vector<VkCommandBuffer> buffers(command_buffers.begin(), command_buffers.end());
			vkFreeCommandBuffers(_device, _command_pool, (ui32)buffers.size(), buffers.data());
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_POOL_HPP */