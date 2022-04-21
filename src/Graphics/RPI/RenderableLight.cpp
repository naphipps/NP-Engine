//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RPI/RenderableLight.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp" //TODO: create OpenGLGraphics.hpp
#endif

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

namespace np::graphics
{
	RenderableLight* RenderableLight::Create(memory::Allocator& allocator, Light& light)
	{
		RenderableLight* renderable_light = nullptr;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			renderable_light = memory::Create<rhi::VulkanRenderableLight>(allocator, light);
			break;
		default:
			break;
		}

		return renderable_light;
	}
} // namespace np::graphics