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
#include "Result.hpp"

namespace np::gpu
{
	struct Frame : public DetailObject
	{
		virtual mem::sptr<ImageResourceView> GetImageResourceView() const = 0;
	};

	struct FrameContext : public DetailObject
	{
		static mem::sptr<FrameContext> Create(mem::sptr<Device> device, const con::vector<DeviceQueueFamily>& queue_families);

		virtual ~FrameContext() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual con::vector<DeviceQueueFamily> GetDeviceQueueFamilies() const = 0;

		virtual void SetAcquireFrameTimeout(siz timeout) = 0;

		virtual Result TryAcquireFrame(mem::sptr<Semaphore> ready_semaphore, mem::sptr<Fence> ready_fence) = 0;

		virtual mem::sptr<Frame> GetPrevAcquiredFrame() const = 0;

		virtual mem::sptr<Frame> GetAcquiredFrame() const = 0;

		virtual siz GetPrevAcquiredFrameIndex() const = 0;

		virtual siz GetAcquiredFrameIndex() const = 0;

		virtual con::vector<mem::sptr<Frame>> GetFrames() const = 0;

		virtual siz GetFrameWidth() const = 0;

		virtual siz GetFrameHeight() const = 0;

		virtual dbl GetFrameAspectRatio() const
		{
			const siz width = GetFrameWidth();
			const siz height = GetFrameHeight();
			return height != 0 ? static_cast<dbl>(width) / static_cast<dbl>(height) : 0;
		}

		virtual Format GetFrameFormat() const = 0;

		virtual void Rebuild() = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_FRAME_CONTEXT_HPP */