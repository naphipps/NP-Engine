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

namespace np::graphics
{
	Scene* Scene::Create(memory::Allocator& allocator, ::entt::registry& ecs_registry, Renderer& renderer)
	{
		Scene* scene = nullptr;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			scene = memory::Create<rhi::VulkanScene>(allocator, ecs_registry, renderer);
			break;
		default:
			break;
		}

		return scene;
	}
} // namespace np::graphics