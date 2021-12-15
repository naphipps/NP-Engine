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

#include "NP-Engine/Window/WindowEvents.hpp"

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanRenderer.hpp"

#if NP_ENGINE_PLATFORM_IS_APPLE
	// TODO: implement

#elif NP_ENGINE_PLATFORM_IS_LINUX
	// TODO: implement

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp"

#endif

#include "Layer.hpp"
#include "Popup.hpp"

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

		virtual void HandleWindowClose(event::Event& event)
		{
			for (graphics::Renderer* renderer : _renderers)
			{
				window::Window* window = event.RetrieveData<window::WindowCloseEvent::DataType>().window;
				renderer->DetachFromWindow(*window);
			}
		}

		virtual void HandleEvent(event::Event& event) override
		{
			switch (event.GetType())
			{
			case event::EventType::GraphicsCreateRendererForWindow:
				HandleCreateRendererForWindow(event);
				break;
			case event::EventType::WindowClose:
				HandleWindowClose(event);
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
			Popup::Select select = Popup::Select::Yes;
			Popup::Buttons buttons = Popup::Buttons::YesNo;

			if (renderers.size() == 0)
			{
				Popup::Show(title, "Could not find suitible renderers.", Popup::Style::Error);
			}
			else
			{
				while (true)
				{
					choose_message = "Click Yes for " + renderers.front()->GetName() + ", or click No to cycle to the next.";
					select = Popup::Show(title, "Available renderers:\n" + available_renderers + choose_message, buttons);

					if (select == Popup::Select::Yes)
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
				// if (!_renderers[i]->IsEnabled()) //TODO: ???
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
			return (event::EventCategory)((ui64)event::EventCategory::Graphics | (ui64)event::EventCategory::Window);
		}

		void Draw(time::DurationMilliseconds time_delta)
		{
			for (graphics::Renderer* renderer : _renderers)
			{
				renderer->Draw(time_delta);
			}
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_GRAPHICS_LAYER_HPP */
