//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RPI/Scene.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp" //TODO: create OpenGLGraphics.hpp
#endif

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

namespace np::graphics
{
	Scene* Scene::Create(memory::Allocator& allocator, Renderer& renderer)
	{
		memory::Block block;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			block = allocator.Allocate(sizeof(rhi::VulkanScene));
			memory::Construct<rhi::VulkanScene>(block, renderer);
			break;
		default:
			break;
		}

		return static_cast<Scene*>(block.ptr);
	}
} // namespace np::graphics