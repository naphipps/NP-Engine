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

#include "Layer.hpp"
#include "ApplicationCloseEvent.hpp"

//TODO: I think we should get rid of all glfw calls here

namespace np::app
{
	class WindowLayer : public Layer
	{
	private:
		//TODO: should this be a set?? I think vector is good for Cleanup(), but we could use a map for fast index lookup
		con::vector<win::Window*> _windows;

	protected:
		virtual void HandleWindowCreate(evnt::Event& e)
		{
			CreateWindow(e.GetData<win::Window::Properties>());
			e.SetHandled();
		}

		virtual void HandleWindowClosing(evnt::Event& e)
		{
			NP_ENGINE_PROFILE_FUNCTION();

			//TODO: what if our windows had a dependency counter (similar to jobs)...
			//TODO: what if WindowClosingEvent had a pointer to our window closing job, and everyone else could add their own dependency
			//TODO: then when we get the event here, we can start our job
			//TODO: then window layer would need to listen for the window closed event, check _windows.size to trigger application close

			win::Window* window = e.GetData<win::WindowClosingEvent::DataType>().window;
			mem::Destroy<win::Window>(_services.GetAllocator(), window);

			for (auto it = _windows.begin(); it != _windows.end(); it++)
				if (*it == window)
				{
					_windows.erase(it);
					break;
				}

			if (_windows.size() == 0)
				_services.GetEventSubmitter().Emplace<ApplicationCloseEvent>();

			e.SetHandled();
		}

		virtual void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::WindowCreate:
				HandleWindowCreate(e);
				break;

			case evnt::EventType::WindowClosing:
				HandleWindowClosing(e);
				break;

			default:
				break;
			}
		}

		void ChooseWindowDetailType()
		{
			win::__detail::RegisteredWindowDetailType = win::WindowDetailType::Glfw;
		}

	public:
		WindowLayer(srvc::Services& services): Layer(services)
		{
			ChooseWindowDetailType();
			win::Window::Init();
		}

		virtual ~WindowLayer()
		{
			for (auto it = _windows.begin(); it != _windows.end(); it++)
				mem::Destroy<win::Window>(_services.GetAllocator(), *it);

			win::Window::Terminate();
		}

		virtual win::Window* CreateWindow(win::Window::Properties& properties)
		{
			return _windows.emplace_back(win::Window::Create(_services, properties));
		}

		virtual void Update(tim::DblMilliseconds time_delta) override
		{
			win::Window::Update();

			for (auto it = _windows.begin(); it != _windows.end(); it++)
				(*it)->Update(time_delta);
		}

		virtual void Cleanup() override
		{
			for (i32 i = _windows.size() - 1; i >= 0; i--)
				if (!_windows[i]->IsRunning())
				{
					mem::Destroy<win::Window>(_services.GetAllocator(), _windows[i]);
					_windows.erase(_windows.begin() + i);
				}

			if (_windows.size() == 0)
				_services.GetEventSubmitter().Emplace<ApplicationCloseEvent>();
		}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
