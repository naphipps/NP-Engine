//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/RenderPipeline.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanRenderPipeline.hpp"

namespace np::gpu
{
	mem::sptr<RenderPipeline> RenderPipeline::Create(Properties properties)
	{
		mem::sptr<RenderPipeline> pipeline = nullptr;

		switch (properties.framebuffers->GetDetailType())
		{
		case DetailType::Vulkan:
			pipeline = mem::create_sptr<__detail::VulkanRenderPipeline>(properties.framebuffers->GetServices()->GetAllocator(), properties);

		default:
			break;
		}

		return pipeline;
	}
}