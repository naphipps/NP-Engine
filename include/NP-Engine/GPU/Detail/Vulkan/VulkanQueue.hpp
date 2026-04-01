//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/7/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_QUEUE_HPP //TODO: fix all GPU ifndef's to NP_ENGINE_GPU_INTERFACE\VULKAN_FILENAME_HPP
#define NP_ENGINE_GPU_VULKAN_QUEUE_HPP

#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanCommandBufferPool.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSemaphore.hpp"
#include "VulkanFence.hpp"
#include "VulkanFrameContext.hpp"

namespace np::gpu::__detail
{
	struct VulkanSubmit
	{
		con::vector<VulkanStage> stages{};
		con::vector<mem::sptr<VulkanCommandBuffer>> commandBuffers{};
		con::vector<mem::sptr<VulkanSemaphore>> waitSemaphores{};
		con::vector<mem::sptr<VulkanSemaphore>> signalSemaphores{};

		VulkanSubmit(const Submit& other = {}):
			stages{other.stages.begin(), other.stages.end()},
			commandBuffers{other.commandBuffers.begin(), other.commandBuffers.end()},
			waitSemaphores{other.waitSemaphores.begin(), other.waitSemaphores.end()},
			signalSemaphores{other.signalSemaphores.begin(), other.signalSemaphores.end()}
		{}

		operator Submit() const
		{
			return {{stages.begin(), stages.end()},
					{commandBuffers.begin(), commandBuffers.end()},
					{waitSemaphores.begin(), waitSemaphores.end()},
					{signalSemaphores.begin(), signalSemaphores.end()}};
		}

		con::vector<VkPipelineStageFlags> GetVkWaitPipelineStageFlags() const
		{
			con::vector<VkPipelineStageFlags> flags(stages.size());
			for (siz i = 0; i < flags.size(); i++)
				flags[i] = stages[i].GetVkPipelineStageFlags();
			return flags;
		}

		con::vector<VkCommandBuffer> GetVkCommandBuffers() const
		{
			con::vector<VkCommandBuffer> command_buffers(commandBuffers.size());
			for (siz i = 0; i < command_buffers.size(); i++)
				command_buffers[i] = *commandBuffers[i];
			return command_buffers;
		}

		con::vector<VkSemaphore> GetVkWaitSemaphores() const
		{
			con::vector<VkSemaphore> semaphores(waitSemaphores.size());
			for (siz i = 0; i < semaphores.size(); i++)
				semaphores[i] = *waitSemaphores[i];
			return semaphores;
		}

		con::vector<VkSemaphore> GetVkSignalSemaphores() const
		{
			con::vector<VkSemaphore> semaphores(signalSemaphores.size());
			for (siz i = 0; i < semaphores.size(); i++)
				semaphores[i] = *signalSemaphores[i];
			return semaphores;
		}

		/*
			NOTE: set pWaitSemaphores, pWaitDstStageMask, pCommandBuffers, pSignalSemaphores alongside this
		*/
		VkSubmitInfo GetVkSubmitInfo() const
		{
			VkSubmitInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			return info;
		}
	};

	struct VulkanPresent
	{
		con::vector<mem::sptr<VulkanSemaphore>> waitSemaphores{};
		con::vector<mem::sptr<VulkanFrameContext>> frameContexts{};

		VulkanPresent(const Present& other):
			waitSemaphores{other.waitSemaphores.begin(), other.waitSemaphores.end()},
			frameContexts{other.frameContexts.begin(), other.frameContexts.end()}
		{}

		operator Present() const
		{
			return {{waitSemaphores.begin(), waitSemaphores.end()}, {frameContexts.begin(), frameContexts.end()}};
		}

		con::vector<VkSemaphore> GetVkWaitSemaphores() const
		{
			con::vector<VkSemaphore> semaphores(waitSemaphores.size());
			for (siz i = 0; i < semaphores.size(); i++)
				semaphores[i] = *waitSemaphores[i];
			return semaphores;
		}

		con::vector<VkSwapchainKHR> GetVkSwapchainKHRs() const
		{
			con::vector<VkSwapchainKHR> swapchains(frameContexts.size());
			for (siz i = 0; i < swapchains.size(); i++)
				swapchains[i] = *frameContexts[i];
			return swapchains;
		}

		con::vector<ui32> GetImageIndices() const
		{
			con::vector<ui32> indices(frameContexts.size());
			for (siz i = 0; i < indices.size(); i++)
				indices[i] = frameContexts[i]->GetAcquiredFrameIndex();
			return indices;
		}

		con::vector<VkResult> GetVkResults() const
		{
			con::vector<VkResult> results(frameContexts.size());
			for (siz i = 0; i < results.size(); i++)
				results[i] = VK_RESULT_MAX_ENUM;
			return results;
		}

		/*
			NOTE: set pWaitSemaphores, pSwapchains, pImageIndices, pResults alongside this
		*/
		VkPresentInfoKHR GetVkPresentInfoKHR() const
		{
			VkPresentInfoKHR info{};
			info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			return info;
		}
	};

	class VulkanQueue : public Queue
	{
	private:
		mem::sptr<VulkanDevice> _device;
		DeviceQueueFamily _family;
		siz _index;
		VkQueue _queue;
		mutexed_wrapper<con::vector<mem::wptr<VulkanCommandBufferPool>>>
			_command_buffer_pools; //TODO: do we want this mutex_wrapped?

		static VkQueue RetrieveVkQueue(mem::sptr<VulkanDevice> device, DeviceQueueFamily family, ui32 index)
		{
			VkQueue queue = nullptr;
			vkGetDeviceQueue(*device->GetLogicalDevice(), family.index, index, &queue);
			return queue;
		}

	public:
		static VkSubmitInfo CreateSubmitInfo()
		{
			VkSubmitInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			return info;
		}

		VulkanQueue(mem::sptr<Device> device, DeviceQueueFamily family, siz index):
			_device(DetailObject::EnsureIsDetailType(device, DetailType::Vulkan)),
			_family(family),
			_index(index),
			_queue(RetrieveVkQueue(_device, _family, _index))
		{}

		virtual ~VulkanQueue()
		{
			vkQueueWaitIdle(_queue);
		}

		operator VkQueue() const
		{
			return _queue;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _device;
		}

		virtual DeviceQueueFamily GetDeviceQueueFamily() const override
		{
			return _family;
		}

		virtual siz GetQueueIndex() const override
		{
			return _index;
		}

		virtual mem::sptr<CommandBufferPool> CreateCommandBufferPool(CommandBufferPoolUsage usage) override
		{
			return mem::create_sptr<VulkanCommandBufferPool>(GetServices()->GetAllocator(), _device->GetLogicalDevice(), usage,
															 _family);
		}

		virtual bl Submit(const gpu::Submit& submit_, mem::sptr<Fence> fence_) override
		{
			const VulkanSubmit submit{submit_};
			const con::vector<VkPipelineStageFlags> wait_flags = submit.GetVkWaitPipelineStageFlags();
			const con::vector<VkCommandBuffer> command_buffers = submit.GetVkCommandBuffers();
			const con::vector<VkSemaphore> wait_semaphores = submit.GetVkWaitSemaphores();
			const con::vector<VkSemaphore> signal_semaphores = submit.GetVkSignalSemaphores();

			VkSubmitInfo info = submit.GetVkSubmitInfo();
			info.pWaitDstStageMask = wait_flags.empty() ? nullptr : wait_flags.data();
			info.commandBufferCount = command_buffers.size();
			info.pCommandBuffers = command_buffers.empty() ? nullptr : command_buffers.data();
			info.waitSemaphoreCount = wait_semaphores.size();
			info.pWaitSemaphores = wait_semaphores.empty() ? nullptr : wait_semaphores.data();
			info.signalSemaphoreCount = signal_semaphores.size();
			info.pSignalSemaphores = signal_semaphores.empty() ? nullptr : signal_semaphores.data();

			mem::sptr<VulkanFence> fence = DetailObject::EnsureIsDetailType(fence_, DetailType::Vulkan);
			VkResult result = vkQueueSubmit(_queue, 1, &info, fence ? *fence : (VkFence) nullptr);
			return result == VK_SUCCESS;
		}

		virtual PresentResults Present(const gpu::Present& present_) override
		{
			const VulkanPresent present{present_};
			const con::vector<VkSemaphore> wait_semaphores = present.GetVkWaitSemaphores();
			const con::vector<VkSwapchainKHR> swapchains = present.GetVkSwapchainKHRs();
			const con::vector<ui32> indices = present.GetImageIndices();
			con::vector<VkResult> vk_results = present.GetVkResults();

			VkPresentInfoKHR info = present.GetVkPresentInfoKHR();
			info.waitSemaphoreCount = wait_semaphores.size();
			info.pWaitSemaphores = wait_semaphores.empty() ? nullptr : wait_semaphores.data();
			info.swapchainCount = swapchains.size();
			info.pSwapchains = swapchains.empty() ? nullptr : swapchains.data();
			info.pImageIndices = indices.empty() ? nullptr : indices.data();
			info.pResults = vk_results.empty() ? nullptr : vk_results.data();

			VkResult vk_result = vkQueuePresentKHR(_queue, &info);
			PresentResults results{};
			results.overallResult = VulkanResult{ vk_result };
			results.individualResults.resize(vk_results.size());
			for (siz i=0; i<vk_results.size(); i++)
				results.individualResults[i] = VulkanResult{ vk_results[i] };
			return results;
		}

		virtual void WaitUntilIdle() const override
		{
			vkQueueWaitIdle(_queue);
		}

		//void WaitUntilIdle() const
		//{
		//	vkQueueWaitIdle(_queue);
		//}

		//VkCommandPoolCreateInfo CreateCommandPoolInfo() const
		//{
		//	VkCommandPoolCreateInfo info = VulkanCommandPool::CreateInfo();
		//	info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		//	return info;
		//}

		//

		//mem::sptr<VulkanCommandPool> CreateCommandPool(VkCommandPoolCreateInfo info)
		//{
		//	info.queueFamilyIndex = GetFamily().index;
		//	mem::sptr<VulkanCommandPool> pool = mem::create_sptr<VulkanCommandPool>(GetServices()->GetAllocator(), _device,
		//info);

		//	CleanupCommandPools();
		//	_command_pools.get_access()->emplace_back(pool);
		//	return pool;
		//}

		//bl Has(mem::sptr<VulkanCommandPool> pool) //TODO: if const is needed, just use sptr for _command_pools
		//{
		//	auto access = _command_pools.get_access();

		//	bl has = false;
		//	for (auto it = access->begin(); !has && it != access->end(); it++)
		//		has |= it->get_sptr() == pool;

		//	return has;
		//}

		//void CleanupCommandPools()
		//{
		//	auto access = _command_pools.get_access();

		//	for (auto it = access->begin(); it != access->end();)
		//	{
		//		if (it->is_expired())
		//		{
		//			it->reset();
		//			it = access->erase(it);
		//		}
		//		else
		//		{
		//			it++;
		//		}
		//	}
		//}

		//VkResult Submit(const con::vector<VkSubmitInfo>& infos, VkFence fence) const
		//{
		//	//TODO: should we check usage here??
		//	return vkQueueSubmit(_queue, (ui32)infos.size(), infos.data(), fence);
		//}

		//VkResult Submit(const con::vector<mem::sptr<VulkanCommandBuffer>>& buffers, VkSubmitInfo info, VkFence fence) const
		//{
		//	con::vector<VkCommandBuffer> command_buffers(buffers.size());
		//	for (siz i = 0; i < buffers.size(); i++)
		//		command_buffers[i] = *buffers[i];

		//	info.commandBufferCount = (ui32)command_buffers.size();
		//	info.pCommandBuffers = command_buffers.data();
		//	return Submit({ info }, fence);
		//}

		//VkResult Present(VkPresentInfoKHR info) const
		//{
		//	return _family.usage.Contains(QueueUsage::Present) ? vkQueuePresentKHR(_queue, &info) : VK_RESULT_MAX_ENUM; //TODO:
		//should we use VK_ERROR_UNKNOWN instead?
		//}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_QUEUE_HPP */