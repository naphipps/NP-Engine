//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Interface/RenderableLight.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/Detail/Vulkan/VulkanGraphics.hpp"

namespace np::gfx
{
	RenderableLight* RenderableLight::Create(srvc::Services& services, Light& light)
	{
		RenderableLight* renderable_light = nullptr;

		switch (__detail::RegisteredGraphicsDetailType.load(mo_acquire))
		{
		case GraphicsDetailType::Vulkan:
			renderable_light = mem::Create<__detail::VulkanRenderableLight>(services.GetAllocator(), services, light);
			break;
		default:
			break;
		}

		return renderable_light;
	}
} // namespace np::gfx