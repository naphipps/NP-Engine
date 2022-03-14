//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/11/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RPI/Renderer.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp" //TODO: create OpenGLGraphics.hpp
#endif

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

namespace np::graphics
{
	Renderer* Renderer::Create(memory::Allocator& allocator, ::entt::registry& ecs_registry)
	{
		memory::Block block;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			block = allocator.Allocate(sizeof(rhi::VulkanRenderer));
			memory::Construct<rhi::VulkanRenderer>(block, ecs_registry);
			break;
		default:
			break;
		}

		return static_cast<Renderer*>(block.ptr);
	}
} // namespace np::graphics