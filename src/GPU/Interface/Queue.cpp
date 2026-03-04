//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/2/26
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/Queue.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanQueue.hpp"

namespace np::gpu
{
	mem::sptr<Queue> Queue::Create(mem::sptr<Device> device, DeviceQueueFamily family, siz index)
	{
		mem::sptr<Queue> queue = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			queue = mem::create_sptr<__detail::VulkanQueue>(device->GetServices()->GetAllocator(), device, family, index);
			break;

		default:
			break;
		}

		return queue;
	}
} // namespace np::gpu