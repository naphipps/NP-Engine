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

#include "NP-Engine/Vendor/EnttInclude.hpp"

#include "NP-Engine/Window/WindowEvents.hpp"

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLGraphics.hpp"
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
		container::vector<graphics::Scene*> _scenes; // TODO: I feel like we need to redesign how we store all these
		container::uset<graphics::Scene*> _unacquired_scenes;
		container::uset<graphics::Scene*> _acquired_scenes;

		virtual void AdjustForWindowClose(event::Event& e)
		{
			window::Window& window = *e.RetrieveData<window::WindowCloseEvent::DataType>().window;

			for (auto it = _scenes.begin(); it != _scenes.end(); it++)
			{
				if ((*it)->GetRenderer().IsAttachedToWindow(window))
				{
					_unacquired_scenes.erase(*it);
					_acquired_scenes.erase(*it);
					memory::Destruct(*it);
					_allocator.Deallocate(*it);
					_scenes.erase(it);
					break;
				}
			}

			for (auto it = _renderers.begin(); it != _renderers.end(); it++)
			{
				if ((*it)->IsAttachedToWindow(window))
				{
					(*it)->DetachFromWindow(window);
					memory::Destruct(*it);
					_allocator.Deallocate(*it);
					_renderers.erase(it);
					break;
				}
			}
		}

		virtual void AdjustForWindowResize(event::Event& e)
		{
			window::Window& window = *e.RetrieveData<window::WindowCloseEvent::DataType>().window;

			for (graphics::Scene*& scene : _scenes)
			{
				if (scene->GetRenderer().IsAttachedToWindow(window))
				{
					scene->AdjustForWindowResize(window);
					break;
				}
			}

			for (graphics::Renderer*& renderer : _renderers)
			{
				if (renderer->IsAttachedToWindow(window))
				{
					renderer->AdjustForWindowResize(window);
					break;
				}
			}
		}

		virtual void HandleEvent(event::Event& e) override
		{
			switch (e.GetType())
			{
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
				if (memory::Construct<graphics::rhi::OpenGLRenderer>(block, _ecs_registry))
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
				if (memory::Construct<graphics::rhi::VulkanRenderer>(block, _ecs_registry))
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

		graphics::Scene* CreateScene(graphics::Renderer& renderer)
		{
			graphics::Scene* scene = graphics::Scene::Create(_allocator, _ecs_registry, renderer);
			_unacquired_scenes.insert(scene);
			return _scenes.emplace_back(scene);
		}

	public:
		GraphicsLayer(event::EventSubmitter& event_submitter, ::entt::registry& ecs_registry):
			Layer(event_submitter, ecs_registry)
		{
			ChooseRhi();
		}

		virtual ~GraphicsLayer()
		{
			for (auto it = _scenes.begin(); it != _scenes.end(); it++)
			{
				memory::Destruct(*it);
				_allocator.Deallocate(*it);
			}

			for (auto it = _renderers.begin(); it != _renderers.end(); it++)
			{
				memory::Destruct(*it);
				_allocator.Deallocate(*it);
			}
		}

		graphics::Renderer* CreateRenderer(window::Window& window)
		{
			graphics::Renderer* renderer = graphics::Renderer::Create(_allocator, _ecs_registry);
			renderer->AttachToWindow(window);
			CreateScene(*renderer);
			return _renderers.emplace_back(renderer);
		}

		virtual event::EventCategory GetHandledCategories() const override
		{
			return (event::EventCategory)((ui64)event::EventCategory::Graphics | (ui64)event::EventCategory::Window);
		}

		graphics::Scene* AcquireScene()
		{
			graphics::Scene* scene = nullptr;

			if (!_unacquired_scenes.empty())
			{
				scene = *_unacquired_scenes.begin();
				_unacquired_scenes.erase(scene);
				_acquired_scenes.insert(scene);
			}

			return scene;
		}

		void ReleaseScene(graphics::Scene* scene)
		{
			if (_acquired_scenes.find(scene) != _acquired_scenes.end())
			{
				_acquired_scenes.erase(scene);
				_unacquired_scenes.insert(scene);
			}
		}

		void Render()
		{
			for (auto it = _acquired_scenes.begin(); it != _acquired_scenes.end(); it++)
				(*it)->Render();
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_GRAPHICS_LAYER_HPP */
