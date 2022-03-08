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

		VkCommandPool CreateCommandPool(VkCommandPoolCreateInfo& info)
		{
			VkCommandPool command_pool = nullptr;

			info.queueFamilyIndex = GetDevice().GetQueueFamilyIndices().graphics.value();

			if (vkCreateCommandPool(GetDevice(), &info, nullptr, &command_pool) != VK_SUCCESS)
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

		VulkanCommandPool(VulkanDevice& device, VkCommandPoolCreateInfo& command_pool_create_info):
			_device(device),
			_command_pool(CreateCommandPool(command_pool_create_info))
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

			if (vkAllocateCommandBuffers(GetDevice(), &command_buffer_allocate_info, buffers.data()) != VK_SUCCESS)
			{
				buffers.clear();
			}

			return container::vector<VulkanCommandBuffer>(buffers.begin(), buffers.end());
		}

		void FreeCommandBuffers(const container::vector<VulkanCommandBuffer>& command_buffers)
		{
			container::vector<VkCommandBuffer> buffers(command_buffers.begin(), command_buffers.end());
			vkFreeCommandBuffers(GetDevice(), _command_pool, (ui32)buffers.size(), buffers.data());
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_POOL_HPP */