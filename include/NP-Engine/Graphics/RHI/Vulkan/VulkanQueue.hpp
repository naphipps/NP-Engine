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

namespace np::gfx::rhi
{
	class VulkanQueue
	{
	private:
		VkDevice _device;
		VkQueue _queue;

		VkQueue RetrieveQueue(ui32 queue_family_index, ui32 queue_index)
		{
			VkQueue queue = nullptr;
			vkGetDeviceQueue(_device, queue_family_index, queue_index, &queue);
			return queue;
		}

	public:
		static VkSubmitInfo CreateSubmitInfo()
		{
			return {VK_STRUCTURE_TYPE_SUBMIT_INFO};
		}

		VulkanQueue(VkDevice device, ui32 queue_family_index, ui32 queue_index):
			_device(device),
			_queue(RetrieveQueue(queue_family_index, queue_index))
		{}

		operator VkQueue() const
		{
			return _queue;
		}

		VkResult Submit(con::vector<VkSubmitInfo> submit_infos, VkFence fence = nullptr)
		{
			return vkQueueSubmit(_queue, (ui32)submit_infos.size(), submit_infos.data(), fence);
		}

		VkResult Submit(con::vector<VulkanCommandBuffer>& command_buffers, VkSubmitInfo submit_info,
						VkFence fence = nullptr)
		{
			con::vector<VkCommandBuffer> buffers(command_buffers.begin(), command_buffers.end());
			submit_info.commandBufferCount = (ui32)buffers.size();
			submit_info.pCommandBuffers = buffers.data();
			return vkQueueSubmit(_queue, 1, &submit_info, fence);
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_QUEUE_HPP */