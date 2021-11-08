//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Renderer/RendererImpl.hpp"

#if NP_ENGINE_PLATFORM_IS_APPLE
//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_LINUX
//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/Renderer/RHI/OpenGL/Renderer.hpp"
#include "NP-Engine/Renderer/RHI/Vulkan/Renderer.hpp"

#endif

//TODO: add summary comments

namespace np::renderer
{
	container::vector<rpi::Renderer*> Renderer::_renderers;


	void Renderer::Init(memory::Allocator& allocator)
	{
		NP_ASSERT(_renderers.empty(), "Cannot init Renderers when we already have some. Shutdown then Init if needed.");

		_renderers.set_allocator(allocator);

		//TODO: read from json file about which renderer was used last, if no file, or not available, ask user which renderer with popups

#if NP_ENGINE_PLATFORM_IS_APPLE
		//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_LINUX
		//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_WINDOWS

		memory::Block block;

		block = _renderers.get_allocator().Allocate(sizeof(rhi::OpenGLRenderer));
		if (block.IsValid())
		{
			memory::Construct<rhi::OpenGLRenderer>(block);
			rhi::OpenGLRenderer* opengl = static_cast<rhi::OpenGLRenderer*>(block.ptr);

			if (opengl->Init())
			{
				_renderers.emplace_back(opengl);
			}
			else
			{
				memory::Destruct(opengl);
				_renderers.get_allocator().Deallocate(block);
			}
		}
		else
		{
			_renderers.get_allocator().Deallocate(block);
		}
		
		block = _renderers.get_allocator().Allocate(sizeof(rhi::VulkanRenderer));
		if (block.IsValid())
		{
			memory::Construct<rhi::VulkanRenderer>(block);
			rhi::VulkanRenderer* vulkan = static_cast<rhi::VulkanRenderer*>(block.ptr);

			if (vulkan->Init())
			{
				_renderers.emplace_back(vulkan);
			}
			else
			{
				memory::Destruct(vulkan);
				_renderers.get_allocator().Deallocate(block);
			}
		}
		else
		{
			_renderers.get_allocator().Deallocate(block);
		}


#endif

		system::Popup::Show("NP-Engine", "Found " + to_str(_renderers.size()) + " supported renderers!");
	}

	void Renderer::Shutdown()
	{
#if NP_ENGINE_PLATFORM_IS_APPLE


#elif NP_ENGINE_PLATFORM_IS_LINUX


#elif NP_ENGINE_PLATFORM_IS_WINDOWS

		for (i32 i = 0; i < _renderers.size(); i++)
		{
			switch (_renderers[i]->GetType())
			{
			case rpi::Renderer::Type::OpenGL:
				memory::Destruct(static_cast<rhi::OpenGLRenderer*>(_renderers[i]));
				break;
			case rpi::Renderer::Type::Vulkan:
				memory::Destruct(static_cast<rhi::VulkanRenderer*>(_renderers[i]));
				break;
			default:
				NP_ASSERT(false, "Detected an invalid renderer type when shutting down...");
				break;
			}

			_renderers.get_allocator().Deallocate(_renderers[i]);
		}


#endif

		_renderers.set_allocator(memory::DefaultAllocator);
		_renderers.clear();
	}

	void Renderer::ResizeWindow(ui32 width, ui32 height)
	{

	}
}