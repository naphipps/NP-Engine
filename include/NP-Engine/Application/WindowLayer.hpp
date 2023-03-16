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
		con::vector<mem::sptr<win::Window>> _windows;

#if NP_ENGINE_PLATFORM_IS_APPLE
		con::mpmc_queue<mem::sptr<win::Window>> _windows_to_destroy;
#endif

	protected:
		virtual void HandleWindowClosing(evnt::Event& e)
		{
			win::WindowClosingEvent& closing_event = (win::WindowClosingEvent&)e;
			win::WindowClosingEvent::DataType& closing_data = closing_event.GetData();
			jsys::JobSystem& job_system = _services.GetJobSystem();

			for (auto it = _windows.begin(); it != _windows.end(); it++)
				if ((*it)->GetUid() == closing_data.windowId)
				{
					// window ownership has gone to the closing job
					closing_data.job->GetDelegate().ConstructData<mem::sptr<win::Window>>(*it);
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
#if NP_ENGINE_PLATFORM_IS_APPLE
			// ownership of window is now resolved in this job procedure by giving it to the window layer for cleanup on the
			// main thread -- because apple is lame and windows MUST be handled on the main thread
			_windows_to_destroy.enqueue(d.GetData<mem::sptr<win::Window>>());

#endif
			// ownership of window is now resolved in this job procedure by destroying it here
			//^ like a normal person unlike apple above
			d.DestructData<mem::sptr<win::Window>>(); //TODO: window seems to not close immediately

			if (_windows.size() == 0)
				_services.GetEventSubmitter().Emplace<ApplicationCloseEvent>();
		}

		virtual void HandleWindowClosed(evnt::Event& e)
		{
			win::WindowClosedEvent& closed_event = (win::WindowClosedEvent&)e;
			win::WindowClosedEvent::DataType& closed_data = closed_event.GetData();
			jsys::JobSystem& job_system = _services.GetJobSystem();

			mem::sptr<jsys::Job> handle_job = job_system.CreateJob();
			// ownership of window is moving from the event to our job procedure
			handle_job->GetDelegate().ConstructData<mem::sptr<win::Window>>(closed_data.window);
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
			_windows.clear();

			Cleanup();

			win::Window::Terminate(win::WindowDetailType::Glfw);
			win::Window::Terminate(win::WindowDetailType::Sdl);
		}

		virtual mem::sptr<win::Window> CreateWindow(win::WindowDetailType detail_type, win::Window::Properties& properties)
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
			mem::sptr<win::Window> window = nullptr;
			while (_windows_to_destroy.try_dequeue(window))
				window.reset();
#endif
		}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
