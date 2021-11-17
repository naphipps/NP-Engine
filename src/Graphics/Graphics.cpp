//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Graphics.hpp"

#if NP_ENGINE_PLATFORM_IS_APPLE
//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_LINUX
//TODO: implement

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp"
#include "NP-Engine/Graphics/RHI/Vulkan/VulkanRenderer.hpp"

#endif

//TODO: add summary comments

namespace np::graphics
{
	void Init()
	{
		container::deque<Renderer*> renderers;
		Renderer* opengl = nullptr;
		Renderer* vulkan = nullptr;

		//TODO: read from json file about which renderer was used last, if no file, or not available, ask user which renderer with popups
		//TODO: where do we store our decision for which renderer we choose??
		//TODO: read a config file to determine renderer order preference

#if /* NP_ENGINE_PLATFORM_IS_LINUX || */ NP_ENGINE_PLATFORM_IS_WINDOWS
		//TODO: try opengl on linux

		opengl = __detail::TryRenderer<rhi::OpenGLRenderer>(renderers.get_allocator());
#endif
#if /* NP_ENGINE_PLATFORM_IS_APPLE || NP_ENGINE_PLATFORM_IS_LINUX || */ NP_ENGINE_PLATFORM_IS_WINDOWS
		//TODO: try vulkan on apple and linux

		vulkan = __detail::TryRenderer<rhi::VulkanRenderer>(renderers.get_allocator());
#endif

		if (vulkan != nullptr) renderers.emplace_back(vulkan);
		if (opengl != nullptr) renderers.emplace_back(opengl);
		
		str available_renderers;

		for (Renderer* r : renderers)
		{
			if (r->IsEnabled())
			{
				available_renderers += "\t- " + r->GetName() + "\n";
			}
		}

		str title = "";
		str choose_message;
		system::Popup::Select select = system::Popup::Select::Yes;
		system::Popup::Buttons buttons = system::Popup::Buttons::YesNo;

		if (renderers.size() == 0)
		{
			system::Popup::Show("NP-Engine", "Could not find suitible renderers.", system::Popup::Style::Error);
		}
		else
		{
			while (true)
			{
				choose_message = "Click Yes for " + renderers.front()->GetName() + ", or click No to cycle to the next.";
				select = system::Popup::Show(title, available_renderers + choose_message, buttons);

				if (select == system::Popup::Select::Yes)
				{
					break;
				}

				renderers.emplace_back(renderers.front());
				renderers.pop_front();
			}

			renderers.front()->RegisterRhiType();
		}

		for (Renderer* r : renderers)
		{
			memory::Destruct(r);
			renderers.get_allocator().Deallocate(r);
		}
	}

	void Shutdown()
	{

	}
}