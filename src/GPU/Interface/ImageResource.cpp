//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/6/26
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/ImageResource.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanImageResource.hpp"

namespace np::gpu
{
	mem::sptr<ImageResource> ImageResource::Create(mem::sptr<Device> device, ImageResourceUsage usage, Format format,
												   siz mip_count, siz layer_count, siz sample_count, siz width, siz height,
												   siz depth, const con::vector<DeviceQueueFamily>& queue_families)
	{
		mem::sptr<ImageResource> image = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			image = mem::create_sptr<__detail::VulkanImageResource>(device->GetServices()->GetAllocator(), device, usage,
																	format, mip_count, layer_count, sample_count, width, height,
																	depth, queue_families);
			break;
		}

		return image;
	}
} // namespace np::gpu