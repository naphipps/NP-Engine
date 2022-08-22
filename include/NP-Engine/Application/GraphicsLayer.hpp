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
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Window/WindowEvents.hpp"

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLGraphics.hpp"
#endif

#include "Layer.hpp"
#include "Popup.hpp"

// TODO: looks like our big-picture is going to expand to support a Task Graph Architecture of some sort...
// TODO: ^ Halcyon's RenderGraph idea is pretty nice, but it is inspired by Frostbite's FrameGraph [O'Donnell 2017]

namespace np::app
{
	class GraphicsLayer : public Layer
	{
	protected:
		container::vector<gfx::Renderer*> _renderers;
		container::vector<gfx::Scene*> _scenes; // TODO: I feel like we need to redesign how we store all these
		container::uset<gfx::Scene*> _unacquired_scenes;
		container::uset<gfx::Scene*> _acquired_scenes;

		virtual void AdjustForWindowClose(evnt::Event& e)
		{
			window::Window& window = *e.RetrieveData<window::WindowCloseEvent::DataType>().window;

			for (auto it = _scenes.begin(); it != _scenes.end(); it++)
				if ((*it)->GetRenderer().IsAttachedToWindow(window))
				{
					_unacquired_scenes.erase(*it);
					_acquired_scenes.erase(*it);
					mem::Destroy<gfx::Scene>(_services.GetAllocator(), *it);
					_scenes.erase(it);
					break;
				}

			for (auto it = _renderers.begin(); it != _renderers.end(); it++)
				if ((*it)->IsAttachedToWindow(window))
				{
					(*it)->DetachFromWindow(window);
					mem::Destroy<gfx::Renderer>(_services.GetAllocator(), *it);
					_renderers.erase(it);
					break;
				}
		}

		virtual void AdjustForWindowResize(evnt::Event& e)
		{
			window::Window& window = *e.RetrieveData<window::WindowCloseEvent::DataType>().window;

			for (gfx::Scene*& scene : _scenes)
				if (scene->GetRenderer().IsAttachedToWindow(window))
				{
					scene->AdjustForWindowResize(window);
					break;
				}

			for (gfx::Renderer*& renderer : _renderers)
				if (renderer->IsAttachedToWindow(window))
				{
					renderer->AdjustForWindowResize(window);
					break;
				}
		}

		virtual void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::WindowClose:
				AdjustForWindowClose(e);
				break;
			case evnt::EventType::WindowResize:
				AdjustForWindowResize(e);
				break;
			default:
				break;
			}
		}

		void ChooseRhi()
		{
			mem::TraitAllocator allocator;
			container::deque<gfx::Renderer*> renderers;
			gfx::Renderer* opengl = nullptr;
			gfx::Renderer* vulkan = nullptr;

			// TODO: we need to redo how we choose our rhi... look at Renderer.IsValid methods or something similar

			// TODO: read graphics config file for renderer preference - then we could put our found renderers into multimap
			// TODO: read from config file about which renderer was used last, if not available, ask user which renderer with
			// popups
			// TODO: store chosen renderer in config file
			// TODO: read a config file to determine renderer order preference

#if NP_ENGINE_PLATFORM_IS_WINDOWS
			// TODO: we're keeping this here for testing purposes - when renderer is complete we can probably remove
			opengl = mem::Create<gfx::rhi::OpenGLRenderer>(allocator, _services);

#endif
			vulkan = mem::Create<gfx::rhi::VulkanRenderer>(allocator, _services);

			if (vulkan != nullptr)
				renderers.emplace_back(vulkan);
			if (opengl != nullptr)
				renderers.emplace_back(opengl);

			str available_renderers;

			for (gfx::Renderer* renderer : renderers)
				available_renderers += "\t- " + renderer->GetName() + "\n";

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
						break;

					renderers.emplace_back(renderers.front());
					renderers.pop_front();
				}

				renderers.front()->RegisterRhiType();
			}

			for (gfx::Renderer* renderer : renderers)
				mem::Destroy<gfx::Renderer>(allocator, renderer);
		}

		gfx::Scene* CreateScene(gfx::Renderer& renderer)
		{
			gfx::Scene* scene = gfx::Scene::Create(_services, renderer);
			_unacquired_scenes.insert(scene);
			return _scenes.emplace_back(scene);
		}

	public:
		GraphicsLayer(srvc::Services& services): Layer(services)
		{
			ChooseRhi();
		}

		virtual ~GraphicsLayer()
		{
			for (auto it = _scenes.begin(); it != _scenes.end(); it++)
				mem::Destroy<gfx::Scene>(_services.GetAllocator(), *it);

			for (auto it = _renderers.begin(); it != _renderers.end(); it++)
				mem::Destroy<gfx::Renderer>(_services.GetAllocator(), *it);
		}

		gfx::Renderer* CreateRenderer(window::Window& window)
		{
			gfx::Renderer* renderer = gfx::Renderer::Create(_services);
			renderer->AttachToWindow(window);
			CreateScene(*renderer);
			return _renderers.emplace_back(renderer);
		}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return (evnt::EventCategory)((ui64)evnt::EventCategory::Graphics | (ui64)evnt::EventCategory::Window);
		}

		gfx::Scene* AcquireScene()
		{
			gfx::Scene* scene = nullptr;

			if (!_unacquired_scenes.empty())
			{
				scene = *_unacquired_scenes.begin();
				_unacquired_scenes.erase(scene);
				_acquired_scenes.insert(scene);
			}

			return scene;
		}

		void ReleaseScene(gfx::Scene* scene)
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
