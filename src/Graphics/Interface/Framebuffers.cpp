//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Interface/Framebuffers.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/Detail/Vulkan/VulkanFramebuffers.hpp"

namespace np::gfx
{
	mem::sptr<Framebuffers> Framebuffers::Create(mem::sptr<RenderPass> render_pass)
	{
		mem::sptr<Framebuffers> framebuffers = nullptr;

		switch (render_pass->GetDetailType())
		{
		case GraphicsDetailType::Vulkan:
			framebuffers = mem::create_sptr<__detail::VulkanFramebuffers>(render_pass->GetServices()->GetAllocator(), render_pass);
			break;

		default:
			break;
		}

		return framebuffers;
	}
}