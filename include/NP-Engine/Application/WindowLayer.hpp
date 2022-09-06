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
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/GlfwInclude.hpp"

#include "Layer.hpp"
#include "ApplicationCloseEvent.hpp"

namespace np::app
{
	class WindowLayer : public Layer
	{
	private:
		mem::TraitAllocator _allocator;
		con::vector<win::Window*> _windows;
		con::vector<void*> _native_windows;

	protected:
		virtual void HandleWindowCreate(evnt::Event& e)
		{
			CreateWindow(e.GetData<win::Window::Properties>());
			e.SetHandled();
		}

		virtual void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::WindowCreate:
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
		WindowLayer(srvc::Services& services): Layer(services)
		{
			glfwInit();
		}

		virtual ~WindowLayer()
		{
			for (auto it = _windows.begin(); it != _windows.end(); it++)
			{
				mem::Destruct(*it);
				_allocator.Deallocate(*it);
			}

			glfwTerminate();
		}

		virtual win::Window* CreateWindow(win::Window::Properties& properties)
		{
			return _windows.emplace_back(mem::Create<win::Window>(_allocator, properties, _services));
		}

		virtual void Update(tim::DblMilliseconds time_delta) override
		{
			glfwPollEvents();

			for (win::Window* window : _windows)
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
					mem::Destroy<win::Window>(_allocator, _windows[i]);
					_windows.erase(_windows.begin() + i);
				}
			}

			if (_windows.size() == 0)
			{
				_services.GetEventSubmitter().Emplace<ApplicationCloseEvent>();
			}
		}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
