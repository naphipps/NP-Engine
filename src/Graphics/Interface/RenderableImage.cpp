//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Interface/RenderableImage.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/Detail/Vulkan/VulkanGraphics.hpp"

namespace np::gfx
{
	RenderableImage* RenderableImage::Create(srvc::Services& services, Image& image)
	{
		RenderableImage* renderable_image = nullptr;

		switch (__detail::RegisteredGraphicsDetailType.load(mo_acquire))
		{
		case GraphicsDetailType::Vulkan:
			renderable_image = mem::Create<__detail::VulkanRenderableImage>(services.GetAllocator(), services, image);
			break;
		default:
			break;
		}

		return renderable_image;
	}
} // namespace np::gfx
