//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/Framebuffer.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanFramebuffer.hpp"

namespace np::gpu
{
	mem::sptr<Framebuffer> Framebuffer::Create(mem::sptr<RenderPass> render_pass, siz width, siz height, siz layer_count, const con::vector<mem::sptr<ImageResourceView>>& views)
	{
		mem::sptr<Framebuffer> framebuffer = nullptr;

		switch (render_pass->GetDetailType())
		{
		case DetailType::Vulkan:
			framebuffer = mem::create_sptr<__detail::VulkanFramebuffer>(render_pass->GetServices()->GetAllocator(), render_pass, width, height, layer_count, views);
			break;

		default:
			break;
		}

		return framebuffer;
	}
} // namespace np::gpu