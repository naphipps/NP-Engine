//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/RenderContext.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanRenderContext.hpp"

namespace np::gpu
{
	mem::sptr<RenderContext> RenderContext::Create(mem::sptr<RenderDevice> device)
	{
		mem::sptr<RenderContext> context = nullptr;

		switch (device->GetDetailType())
		{
		case GraphicsDetailType::Vulkan:
			context = mem::create_sptr<__detail::VulkanRenderContext>(device->GetServices()->GetAllocator(), device);
			break;

		default:
			break;
		}

		return context;
	}
}