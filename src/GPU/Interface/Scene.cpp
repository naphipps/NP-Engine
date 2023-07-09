//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/Scene.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanScene.hpp"

namespace np::gpu
{
	mem::sptr<Scene> Scene::Create(Properties properties)
	{
		mem::sptr<Scene> scene = nullptr;

		switch (properties.renderPipeline->GetDetailType())
		{
		case DetailType::Vulkan:
			scene =
				mem::create_sptr<__detail::VulkanScene>(properties.renderPipeline->GetServices()->GetAllocator(), properties);
			break;

		default:
			break;
		}

		return scene;
	}
} // namespace np::gpu