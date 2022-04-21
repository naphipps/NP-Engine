//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RPI/RenderableImage.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp" //TODO: create OpenGLGraphics.hpp
#endif

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

namespace np::graphics
{
	RenderableImage* RenderableImage::Create(memory::Allocator& allocator, Image& image)
	{
		RenderableImage* renderable_image = nullptr;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			renderable_image = memory::Create<rhi::VulkanRenderableImage>(allocator, image);
			break;
		default:
			break;
		}

		return renderable_image;
	}
} // namespace np::graphics
