//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/11/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Interface/Renderer.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/Detail/Vulkan/VulkanGraphics.hpp"

namespace np::gfx
{
	Renderer* Renderer::Create(srvc::Services& services)
	{
		Renderer* renderer = nullptr;

		switch (__detail::RegisteredGraphicsDetailType.load(mo_acquire))
		{
		case GraphicsDetailType::Vulkan:
			renderer = mem::Create<__detail::VulkanRenderer>(services.GetAllocator(), services);
			break;
		default:
			break;
		}

		return renderer;
	}

} // namespace np::gfx