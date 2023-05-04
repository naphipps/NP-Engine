//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Interface/RenderTarget.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/Detail/Vulkan/VulkanRenderTarget.hpp"

namespace np::gpu
{
	mem::sptr<RenderTarget> RenderTarget::Create(mem::sptr<DetailInstance> instance, mem::sptr<win::Window> window)
	{
		mem::sptr<RenderTarget> target = nullptr;

		switch (instance->GetDetailType())
		{
		case GraphicsDetailType::Vulkan:
			target = mem::create_sptr<__detail::VulkanRenderTarget>(instance->GetServices()->GetAllocator(), instance, window);
			break;

		default:
			break;
		}

		return target;
	}
}