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

// TODO: I think we should get rid of all glfw calls here

namespace np::app
{
	class WindowLayer : public Layer
	{
	private:
		// TODO: should this be a set?? I think vector is good for Cleanup(), but we could use a map for fast index lookup
		con::vector<win::Window*> _windows;

#if NP_ENGINE_PLATFORM_IS_APPLE
		con::mpmc_queue<win::Window*> _windows_to_destroy;
#endif

	protected:
		virtual void HandleWindowClosing(evnt::Event& e)
		{
			win::WindowClosingEvent::DataType& closing_data = e.GetData<win::WindowClosingEvent::DataType>();
			jsys::JobSystem& job_system = _services.GetJobSystem();

			// window ownership has gone to the closing job
			for (auto it = _windows.begin(); it != _windows.end(); it++)
				if (*it == closing_data.window)
				{
					_windows.erase(it);
					break;
				}

			// our closing job must be submitted here
			job_system.SubmitJob(jsys::JobPriority::Normal, closing_data.job);
			closing_data.job.reset();
			e.SetHandled();
		}

		static void HandleWindowClosedCallback(void* caller, mem::Delegate& d)
		{
			((WindowLayer*)caller)->HandleWindowClosedProcedure(d);
		}

		void HandleWindowClosedProcedure(mem::Delegate& d)
		{
			win::Window* closed_window = d.GetData<win::Window*>();

#if NP_ENGINE_PLATFORM_IS_APPLE
			// ownership of window is now resolved in this job procedure by giving it to the window layer for cleanup on the
			// main thread -- because apple is lame and windows MUST be handled on the main thread
			_windows_to_destroy.enqueue(closed_window);

#else
			// ownership of window is now resolved in this job procedure by destroying it here
			//^ like a normal person unlike apple above
			mem::Destroy<win::Window>(_services.GetAllocator(), closed_window);//TODO: window seems to not close immediately

#endif
			if (_windows.size() == 0)
				_services.GetEventSubmitter().Emplace<ApplicationCloseEvent>();
		}

		virtual void HandleWindowClosed(evnt::Event& e)
		{
			win::WindowClosedEvent::DataType& closed_data = e.GetData<win::WindowClosedEvent::DataType>();
			jsys::JobSystem& job_system = _services.GetJobSystem();

			mem::sptr<jsys::Job> handle_job = job_system.CreateJob();
			// ownership of window is moving from the event to our job procedure
			handle_job->GetDelegate().SetData<win::Window*>(closed_data.window);
			handle_job->GetDelegate().SetCallback(this, HandleWindowClosedCallback);
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

	public:
		WindowLayer(srvc::Services& services): Layer(services)
		{
			win::Window::Init(win::WindowDetailType::Glfw);
			win::Window::Init(win::WindowDetailType::Sdl);
		}

		virtual ~WindowLayer()
		{
			for (auto it = _windows.begin(); it != _windows.end(); it++)
				mem::Destroy<win::Window>(_services.GetAllocator(), *it);

			Cleanup();

			win::Window::Terminate(win::WindowDetailType::Glfw);
			win::Window::Terminate(win::WindowDetailType::Sdl);
		}

		virtual win::Window* CreateWindow(win::WindowDetailType detail_type, win::Window::Properties& properties)
		{
			return _windows.emplace_back(win::Window::Create(detail_type, _services, properties));
		}

		virtual void Update(tim::DblMilliseconds time_delta) override
		{
			win::Window::Update(win::WindowDetailType::Glfw);
			win::Window::Update(win::WindowDetailType::Sdl);

			for (auto it = _windows.begin(); it != _windows.end(); it++)
				(*it)->Update(time_delta);
		}

		virtual void Cleanup() override
		{
#if NP_ENGINE_PLATFORM_IS_APPLE
			win::Window* window = nullptr;
			while (_windows_to_destroy.try_dequeue(window))
				mem::Destroy<win::Window>(_services.GetAllocator(), window);
#endif
		}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
