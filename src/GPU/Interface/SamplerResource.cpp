//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/5/26
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/SamplerResource.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanSamplerResource.hpp"

namespace np::gpu
{
	mem::sptr<SamplerResource> SamplerResource::Create(mem::sptr<Device> device, SamplerResourceUsage usage, dbl anisotrophy,
													   CompareOperation op, LodBounds lod_bounds, SamplerBorder border,
													   SamplerAddressModes address_modes)
	{
		mem::sptr<SamplerResource> resource = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			resource = mem::create_sptr<__detail::VulkanSamplerResource>(device->GetServices()->GetAllocator(), device, usage,
																		 anisotrophy, op, lod_bounds, border, address_modes);
			break;
		}

		return resource;
	}
} // namespace np::gpu