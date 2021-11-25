//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/11/21
//
//##===----------------------------------------------------------------------===##//

#include <iostream> //TODO: remove

#include "NP-Engine/Graphics/RPI/Renderer.hpp"

#if NP_ENGINE_PLATFORM_IS_APPLE
//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_LINUX
//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp"

#endif

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanRenderer.hpp"

namespace np::graphics
{
	Renderer* Renderer::Create(memory::Allocator& allocator)
	{
		memory::Block block;

		switch (__detail::RegisteredRhiType)
		{
		case RhiType::Vulkan: //TODO: seems to work on all platforms...
			block = allocator.Allocate(sizeof(rhi::VulkanRenderer));
			memory::Construct<rhi::VulkanRenderer>(block);
			break;
        default:
            break;
		}

		return (Renderer*)block.ptr;
	}
}