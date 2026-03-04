//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/BufferResource.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanBufferResource.hpp"

namespace np::gpu
{
	mem::sptr<BufferResource> BufferResource::Create(mem::sptr<Device> device, BufferResourceUsage usage, siz size, const con::vector<DeviceQueueFamily>& queue_families)
	{
		mem::sptr<BufferResource> resource = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			resource = mem::create_sptr<__detail::VulkanBufferResource>(device->GetServices()->GetAllocator(), device, usage, size, queue_families);
			break;
		}

		return resource;
	}
} // namespace np::gpu