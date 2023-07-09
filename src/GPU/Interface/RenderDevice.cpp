//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/RenderDevice.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanRenderDevice.hpp"

namespace np::gpu
{
	mem::sptr<RenderDevice> RenderDevice::Create(mem::sptr<RenderTarget> target)
	{
		mem::sptr<RenderDevice> device = nullptr;

		switch (target->GetDetailType())
		{
		case DetailType::Vulkan:
			device = mem::create_sptr<__detail::VulkanRenderDevice>(target->GetServices()->GetAllocator(), target);
			break;

		default:
			break;
		}

		return device;
	}
} // namespace np::gpu