//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_LAYER_HPP
#define NP_ENGINE_GRAPHICS_LAYER_HPP

#include "NP-Engine/Graphics/Graphics.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanRenderer.hpp"

#if NP_ENGINE_PLATFORM_IS_APPLE
	// TODO: implement

#elif NP_ENGINE_PLATFORM_IS_LINUX
	// TODO: implement

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp"

#endif

#include "Layer.hpp"

namespace np::app
{
	class GraphicsLayer : public Layer
	{
	protected:
		memory::TraitAllocator _allocator;
		container::vector<graphics::Renderer*> _renderers;

		virtual void HandleCreateRendererForWindow(event::Event& event)
		{
			graphics::Renderer* renderer = CreateRenderer();
			window::Window* window = event.RetrieveData<graphics::GraphicsCreateRendererForWindowEvent::DataType>().window;
			renderer->AttachToWindow(*window);
			event.SetHandled();
		}

		virtual void HandleEvent(event::Event& event) override
		{
			switch (event.GetType())
			{
			case event::EVENT_TYPE_GRAPHICS_CREATE_RENDERER_FOR_WINDOW:
				HandleCreateRendererForWindow(event);
				break;
			}
		}

		void ChooseRhi()
		{
			memory::Block block;
			container::deque<graphics::Renderer*> renderers;
			graphics::Renderer* opengl = nullptr;
			graphics::Renderer* vulkan = nullptr;
			memory::Allocator& allocator = memory::DefaultTraitAllocator;

			// TODO: read graphics config file for renderer preference - then we could put our found renderers into multimap

			// TODO: read from json file about which renderer was used last, if no file, or not available, ask user which
			// renderer with popups
			// TODO: where do we store our decision for which renderer we choose??
			// TODO: read a config file to determine renderer order preference

#if /* NP_ENGINE_PLATFORM_IS_LINUX || */ NP_ENGINE_PLATFORM_IS_WINDOWS
			// TODO: try opengl on linux

			block = allocator.Allocate(sizeof(graphics::rhi::OpenGLRenderer));
			if (block.IsValid())
			{
				if (memory::Construct<graphics::rhi::OpenGLRenderer>(block))
				{
					opengl = static_cast<graphics::Renderer*>(block.ptr);
				}
				else
				{
					allocator.Deallocate(block);
				}
			}
#endif
#if NP_ENGINE_PLATFORM_IS_APPLE || /* NP_ENGINE_PLATFORM_IS_LINUX || */ NP_ENGINE_PLATFORM_IS_WINDOWS
			// TODO: try vulkan on apple and linux

			block = allocator.Allocate(sizeof(graphics::rhi::VulkanRenderer));
			if (block.IsValid())
			{
				if (memory::Construct<graphics::rhi::VulkanRenderer>(block))
				{
					vulkan = static_cast<graphics::Renderer*>(block.ptr);
				}
				else
				{
					allocator.Deallocate(block);
				}
			}
#endif

			if (vulkan != nullptr)
				renderers.emplace_back(vulkan);
			if (opengl != nullptr)
				renderers.emplace_back(opengl);

			str available_renderers;

			for (graphics::Renderer* renderer : renderers)
			{
				// if (renderer->IsEnabled()) //TODO: !!!
				{
					available_renderers += "\t- " + renderer->GetName() + "\n";
				}
			}

			str title = "NP Engine";
			str choose_message;
			system::Popup::Select select = system::Popup::Select::Yes;
			system::Popup::Buttons buttons = system::Popup::Buttons::YesNo;

			if (renderers.size() == 0)
			{
				system::Popup::Show(title, "Could not find suitible renderers.", system::Popup::Style::Error);
			}
			else
			{
				while (true)
				{
					choose_message = "Click Yes for " + renderers.front()->GetName() + ", or click No to cycle to the next.";
					select =
						system::Popup::Show(title, "Available renderers:\n" + available_renderers + choose_message, buttons);

					if (select == system::Popup::Select::Yes)
					{
						break;
					}

					renderers.emplace_back(renderers.front());
					renderers.pop_front();
				}

				renderers.front()->RegisterRhiType();
			}

			for (graphics::Renderer* renderer : renderers)
			{
				memory::Destruct(renderer);
				allocator.Deallocate(renderer);
			}
		}

	public:
		GraphicsLayer(event::EventSubmitter& event_submitter): Layer(event_submitter)
		{
			ChooseRhi();
		}

		virtual ~GraphicsLayer()
		{
			for (auto it = _renderers.begin(); it != _renderers.end(); it++)
			{
				memory::Destruct(*it);
				_allocator.Deallocate(*it);
			}
		}

		graphics::Renderer* CreateRenderer()
		{
			graphics::Renderer* renderer = graphics::Renderer::Create(_allocator);
			_renderers.emplace_back(renderer);
			return renderer;
		}

		virtual void Cleanup() override
		{
			for (i32 i = _renderers.size() - 1; i >= 0; i--)
			{
				// if (!_renderers[i]->IsEnabled()) //TODO: !!!
				if (false)
				{
					graphics::Renderer* renderer = _renderers[i];
					_renderers.erase(_renderers.begin() + i);
					memory::Destruct(renderer);
					_allocator.Deallocate(renderer);
				}
			}
		}

		virtual event::EventCategory GetHandledCategories() const override
		{
			return event::EVENT_CATEGORY_GRAPHICS;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_GRAPHICS_LAYER_HPP */
