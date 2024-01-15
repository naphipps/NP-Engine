//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/7/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_QUEUE_HPP
#define NP_ENGINE_VULKAN_QUEUE_HPP

#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommandBuffer.hpp"
#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanQueue
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkQueue _queue;

		static VkQueue RetrieveQueue(mem::sptr<VulkanLogicalDevice> device, ui32 queue_family_index, ui32 queue_index)
		{
			VkQueue queue = nullptr;
			vkGetDeviceQueue(*device, queue_family_index, queue_index, &queue);
			return queue;
		}

	public:
		static VkSubmitInfo CreateSubmitInfo()
		{
			return {VK_STRUCTURE_TYPE_SUBMIT_INFO};
		}

		VulkanQueue(mem::sptr<VulkanLogicalDevice> device, ui32 queue_family_index, ui32 queue_index):
			_device(device),
			_queue(RetrieveQueue(_device, queue_family_index, queue_index))
		{}

		virtual ~VulkanQueue()
		{
			vkQueueWaitIdle(_queue);
		}

		operator VkQueue() const
		{
			return _queue;
		}

		VkResult Submit(con::vector<VkSubmitInfo> submit_infos, VkFence fence = nullptr)
		{
			return vkQueueSubmit(_queue, (ui32)submit_infos.size(), submit_infos.data(), fence);
		}

		VkResult Submit(mem::sptr<VulkanCommandBuffer> command_buffer, VkSubmitInfo submit_info, VkFence fence = nullptr)
		{
			return Submit(con::vector<mem::sptr<VulkanCommandBuffer>>{command_buffer}, submit_info, fence);
		}

		VkResult Submit(const con::vector<mem::sptr<VulkanCommandBuffer>>& command_buffers, VkSubmitInfo submit_info,
						VkFence fence = nullptr)
		{
			con::vector<VkCommandBuffer> buffers(command_buffers.size());
			for (siz i = 0; i < buffers.size(); i++)
				buffers[i] = *command_buffers[i];

			submit_info.commandBufferCount = (ui32)buffers.size();
			submit_info.pCommandBuffers = buffers.data();
			return vkQueueSubmit(_queue, 1, &submit_info, fence);
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_QUEUE_HPP */