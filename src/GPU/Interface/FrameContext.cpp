//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/FrameContext.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanFrameContext.hpp"

namespace np::gpu
{
	mem::sptr<FrameContext> FrameContext::Create(mem::sptr<Device> device, const con::vector<DeviceQueueFamily>& queue_families)
	{
		mem::sptr<FrameContext> context = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			context =
				mem::create_sptr<__detail::VulkanFrameContext>(device->GetServices()->GetAllocator(), device, queue_families);
			break;

		default:
			break;
		}

		return context;
	}
} // namespace np::gpu