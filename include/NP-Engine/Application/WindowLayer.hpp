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

		virtual void HandleWindowClosing(evnt::Event& e)
		{
			win::WindowClosingEvent::DataType& closing_data = e.GetData<win::WindowClosingEvent::DataType>();
			jsys::JobSystem& job_system = _services.GetJobSystem();

			//window ownership has gone to the closing job
			for (auto it = _windows.begin(); it != _windows.end(); it++)
				if (*it == closing_data.window)
				{
					_windows.erase(it);
					break;
				}

			//our closing job must be submitted here
			job_system.SubmitJob(jsys::JobPriority::Normal, closing_data.job);
			e.SetHandled();
		}

		void HandleWindowClosedProcedure(mem::Delegate& d)
		{
			//ownership of window is now resolved in this job procedure by destroying it here
			mem::Destroy<win::Window>(_services.GetAllocator(), d.GetData<win::Window*>());

			if (_windows.size() == 0)
				_services.GetEventSubmitter().Emplace<ApplicationCloseEvent>();
		}

		virtual void HandleWindowClosed(evnt::Event& e)
		{
			win::WindowClosedEvent::DataType& closed_data = e.GetData<win::WindowClosedEvent::DataType>();
			jsys::JobSystem& job_system = _services.GetJobSystem();

			mem::Delegate procedure{};
			//ownership of window is moving from the event to our job procedure
			procedure.SetData<win::Window*>(closed_data.window);
			procedure.Connect<WindowLayer, &WindowLayer::HandleWindowClosedProcedure>(this);

			jsys::Job* handle_job = job_system.CreateJob(::std::move(procedure));
			job_system.SubmitJob(jsys::JobPriority::Higher, handle_job);

			e.SetHandled();
		}

		virtual void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::WindowClosing:
				HandleWindowClosing(e);
				break;

			case evnt::EventType::WindowClosed:
				HandleWindowClosed(e);
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

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
