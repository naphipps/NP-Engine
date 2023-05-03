//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Interface/RenderPipeline.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/Detail/Vulkan/VulkanRenderPipeline.hpp"

namespace np::gfx
{
	mem::sptr<RenderPipeline> RenderPipeline::Create(Properties properties)
	{
		mem::sptr<RenderPipeline> pipeline = nullptr;

		switch (properties.framebuffers->GetDetailType())
		{
		case GraphicsDetailType::Vulkan:
			pipeline = mem::create_sptr<__detail::VulkanRenderPipeline>(properties.framebuffers->GetServices()->GetAllocator(), properties);

		default:
			break;
		}

		return pipeline;
	}
}