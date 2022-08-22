//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RPI/Scene.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

namespace np::gfx
{
	Scene* Scene::Create(srvc::Services& services, Renderer& renderer)
	{
		Scene* scene = nullptr;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			scene = mem::Create<rhi::VulkanScene>(services.GetAllocator(), services, renderer);
			break;
		default:
			break;
		}

		return scene;
	}
} // namespace np::gfx