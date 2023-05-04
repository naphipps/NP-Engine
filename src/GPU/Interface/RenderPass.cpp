//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/RenderPass.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanRenderPass.hpp"

namespace np::gpu
{
	mem::sptr<RenderPass> RenderPass::Create(mem::sptr<RenderContext> context)
	{
		mem::sptr<RenderPass> pass = nullptr;

		switch (context->GetDetailType())
		{
		case GraphicsDetailType::Vulkan:
			pass = mem::create_sptr<__detail::VulkanRenderPass>(context->GetServices()->GetAllocator(), context);
			break;

		default:
			break;
		}

		return pass;
	}
}