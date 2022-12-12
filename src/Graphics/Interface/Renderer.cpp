//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/11/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RPI/Renderer.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

namespace np::gfx
{
	Renderer* Renderer::Create(srvc::Services& services)
	{
		Renderer* renderer = nullptr;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			renderer = mem::Create<rhi::VulkanRenderer>(services.GetAllocator(), services);
			break;
		default:
			break;
		}

		return renderer;
	}

} // namespace np::gfx