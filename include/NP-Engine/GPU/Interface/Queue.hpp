//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/2/25
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_QUEUE_HPP
#define NP_ENGINE_GPU_INTERFACE_QUEUE_HPP

#include "NP-Engine/Memory/Memory.hpp"

#include "CommandBuffer.hpp"
#include "CommandBufferPool.hpp"
#include "Stage.hpp"
#include "Semaphore.hpp"
#include "Fence.hpp"
#include "FrameContext.hpp"
#include "Device.hpp"
#include "Result.hpp"

namespace np::gpu
{
	struct Submit
	{
		con::vector<Stage> stages{};
		con::vector<mem::sptr<CommandBuffer>> commandBuffers{};
		con::vector<mem::sptr<Semaphore>> waitSemaphores{};
		con::vector<mem::sptr<Semaphore>> signalSemaphores{};
	};

	struct Present
	{
		con::vector<mem::sptr<Semaphore>> waitSemaphores{};
		con::vector<mem::sptr<FrameContext>> frameContexts{};
	};

	struct PresentResults
	{
		Result overallResult{};
		con::vector<Result> individualResults{};
	};

	struct Queue : public DetailObject
	{
		static mem::sptr<Queue> Create(mem::sptr<Device> device, DeviceQueueFamily family, siz index);

		virtual ~Queue() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual DeviceQueueFamily GetDeviceQueueFamily() const = 0;

		virtual siz GetQueueIndex() const = 0; //TODO: do we want to rename this?

		virtual mem::sptr<CommandBufferPool> CreateCommandBufferPool(CommandBufferPoolUsage usage) = 0;

		virtual bl Submit(const Submit& submit, mem::sptr<Fence> fence) = 0; //TODO: how are we going to do this?

		/*
			returns vector<bl> since we accept vector<sptr<FrameContext>>
				each bl is the success boolean for each FrameContext presentation attempt
		*/
		virtual PresentResults Present(const Present& present) = 0;

		virtual void WaitUntilIdle() const = 0;
	};
} //namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_QUEUE_HPP */