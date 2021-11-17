//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/11/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RPI/Renderer.hpp"

#if NP_ENGINE_PLATFORM_IS_APPLE
//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_LINUX
//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp"
#include "NP-Engine/Graphics/RHI/Vulkan/VulkanRenderer.hpp"

#endif

namespace np::graphics
{
	Renderer::RhiType Renderer::_renderer_rhi_type = Renderer::RhiType::None;

	Renderer* Renderer::Create(memory::Allocator& allocator)
	{
		Renderer* renderer = nullptr;

		switch (GetRegisteredRhiType())
		{
#if /* NP_ENGINE_PLATFORM_IS_APPLE || NP_ENGINE_PLATFORM_IS_LINUX || */ NP_ENGINE_PLATFORM_IS_WINDOWS
			//TODO: try vulkan on apple and linux
		case RhiType::Vulkan:
			memory::Block block = allocator.Allocate(sizeof(rhi::VulkanRenderer));
			memory::Construct<rhi::VulkanRenderer>(block);
			renderer = (Renderer*)block.Begin();
			break;
#endif
		}

		return renderer;
	}
}