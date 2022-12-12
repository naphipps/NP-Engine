//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Interface/Scene.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/Detail/Vulkan/VulkanGraphics.hpp"

namespace np::gfx
{
	Scene* Scene::Create(srvc::Services& services, Renderer& renderer)
	{
		Scene* scene = nullptr;

		switch (__detail::RegisteredGraphicsDetailType.load(mo_acquire))
		{
		case GraphicsDetailType::Vulkan:
			scene = mem::Create<__detail::VulkanScene>(services.GetAllocator(), services, renderer);
			break;
		default:
			break;
		}

		return scene;
	}
} // namespace np::gfx