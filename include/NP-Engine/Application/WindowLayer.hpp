//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/16/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_LAYER_HPP
#define NP_ENGINE_WINDOW_LAYER_HPP

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/GlfwInclude.hpp"
#include "NP-Engine/Vendor/EnttInclude.hpp"

#include "Layer.hpp"
#include "ApplicationCloseEvent.hpp"

namespace np::app
{
	class WindowLayer : public Layer
	{
	private:
		memory::TraitAllocator _allocator;
		container::vector<window::Window*> _windows;
		container::vector<void*> _native_windows;

	protected:
		virtual void HandleWindowCreate(event::Event& e)
		{
			CreateWindow(e.RetrieveData<window::Window::Properties>());
			e.SetHandled();
		}

		virtual void HandleEvent(event::Event& e) override
		{
			switch (e.GetType())
			{
			case event::EventType::WindowCreate:
				HandleWindowCreate(e);
				break;
			default:
				break;
			}

			for (auto it = _windows.begin(); !e.IsHandled() && it != _windows.end(); it++)
			{
				(*it)->OnEvent(e);
			}
		}

	public:
		WindowLayer(event::EventSubmitter& event_submitter, ::entt::registry& ecs_registry):
			Layer(event_submitter, ecs_registry)
		{
			glfwInit();
		}

		virtual ~WindowLayer()
		{
			for (auto it = _windows.begin(); it != _windows.end(); it++)
			{
				memory::Destruct(*it);
				_allocator.Deallocate(*it);
			}

			glfwTerminate();
		}

		virtual window::Window* CreateWindow(window::Window::Properties& properties)
		{
			return _windows.emplace_back(memory::Create<window::Window>(_allocator, properties, _event_submitter));
		}

		virtual void Update(time::DurationMilliseconds time_delta) override
		{
			glfwPollEvents();

			for (window::Window* window : _windows)
			{
				window->Update(time_delta);
			}
		}

		virtual void Cleanup() override
		{
			for (i32 i = _windows.size() - 1; i >= 0; i--)
			{
				if (!_windows[i]->IsRunning())
				{
					memory::Destroy<window::Window>(_allocator, _windows[i]);
					_windows.erase(_windows.begin() + i);
				}
			}

			if (_windows.size() == 0)
			{
				_event_submitter.Emplace<ApplicationCloseEvent>();
			}
		}

		virtual event::EventCategory GetHandledCategories() const override
		{
			return event::EventCategory::Window;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
