//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_FRAME_CONTEXT_HPP
#define NP_ENGINE_GPU_INTERFACE_FRAME_CONTEXT_HPP

#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "ImageResourceView.hpp"
#include "Fence.hpp"
#include "Semaphore.hpp"
#include "Device.hpp"

namespace np::gpu
{
	struct Frame : public DetailObject
	{
		virtual mem::sptr<ImageResourceView> GetImageResourceView() const = 0;

		virtual mem::sptr<Semaphore> GetReadySemaphore() const = 0;

		virtual mem::sptr<Fence> GetReadyFence() const = 0;

		virtual mem::sptr<Semaphore> GetCompletedSemaphore() const = 0;
	};

	struct FrameContext : public DetailObject
	{
		static mem::sptr<FrameContext> Create(mem::sptr<Device> device, const con::vector<DeviceQueueFamily>& queue_families);

		virtual ~FrameContext() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const = 0;

		virtual void SetAcquireFrameTimeout(siz timeout) = 0;

		virtual bl TryAcquireFrame() = 0;

		virtual mem::sptr<Frame> GetPrevAcquiredFrame() const = 0;

		virtual mem::sptr<Frame> GetAcquiredFrame() const = 0;

		virtual siz GetPrevAcquiredFrameIndex() const = 0;

		virtual siz GetAcquiredFrameIndex() const = 0;

		virtual con::vector<mem::sptr<Frame>> GetFrames() const = 0;

		virtual siz GetFrameWidth() const = 0;

		virtual siz GetFrameHeight() const = 0;

		virtual Format GetFrameFormat() const = 0;

		//TODO: do we need a way to get images/views?

		virtual con::vector<mem::sptr<ImageResourceView>> GetImageViews() const
		{
			const con::vector<mem::sptr<Frame>> frames = GetFrames();
			con::vector<mem::sptr<ImageResourceView>> views(frames.size());
			for (siz i = 0; i < views.size(); i++)
				views[i] = frames[i]->GetImageResourceView();
			return views;
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_FRAME_CONTEXT_HPP */