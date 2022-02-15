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

#if NP_ENGINE_PLATFORM_IS_APPLE || NP_ENGINE_PLATFORM_IS_LINUX
	// TODO: determine if we need this after our renderer is complete

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

		virtual void HandleCreateRendererForWindow(event::Event& e)
		{
			graphics::Renderer* renderer = CreateRenderer();
			renderer->AttachToWindow(*e.RetrieveData<graphics::GraphicsCreateRendererForWindowEvent::DataType>().window);
			e.SetHandled();
		}

		virtual void AdjustForWindowClose(event::Event& e)
		{
			for (graphics::Renderer* renderer : _renderers)
			{
				renderer->DetachFromWindow(*e.RetrieveData<window::WindowCloseEvent::DataType>().window);
			}
		}

		virtual void AdjustForWindowResize(event::Event& e)
		{
			for (graphics::Renderer* renderer : _renderers)
			{
				renderer->AdjustForWindowResize(*e.RetrieveData<window::WindowResizeEvent::DataType>().window);
			}
		}

		virtual void HandleEvent(event::Event& e) override
		{
			switch (e.GetType())
			{
			case event::EventType::GraphicsCreateRendererForWindow:
				HandleCreateRendererForWindow(e);
				break;
			case event::EventType::WindowClose:
				AdjustForWindowClose(e);
				break;
			case event::EventType::WindowResize:
				AdjustForWindowResize(e);
				break;
			default:
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
			// TODO: read from config file about which renderer was used last, if not available, ask user which renderer with
			// popups
			// TODO: store chosen renderer in config file
			// TODO: read a config file to determine renderer order preference

#if NP_ENGINE_PLATFORM_IS_WINDOWS
			// TODO: we're keeping this here for testing purposes - when renderer is complete we can probably remove
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
				available_renderers += "\t- " + renderer->GetName() + "\n";
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
			return _renderers.emplace_back(graphics::Renderer::Create(_allocator));
		}

		virtual event::EventCategory GetHandledCategories() const override
		{
			return (event::EventCategory)((ui64)event::EventCategory::Graphics | (ui64)event::EventCategory::Window);
		}

		void Draw()
		{
			for (graphics::Renderer* renderer : _renderers)
				renderer->Draw();
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_GRAPHICS_LAYER_HPP */
