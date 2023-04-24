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

namespace np::app
{
	class WindowLayer : public Layer
	{
	private:
		Mutex _windows_mutex;
		con::vector<mem::wptr<win::Window>> _windows;
		con::mpmc_queue<mem::sptr<win::Window>> _windows_to_destroy;

	protected:
		static void AdjustForWindowClosingCallback(void* caller, mem::Delegate& d)
		{
			((WindowLayer*)caller)->AdjustForWindowClosingProcedure(d);
		}

		void AdjustForWindowClosingProcedure(mem::Delegate& d)
		{
			uid::Uid windowId = d.GetData<uid::Uid>();
			mem::sptr<win::Window> window = nullptr;
			{
				Lock l(_windows_mutex);
				for (auto it = _windows.begin(); it != _windows.end(); it++)
				{
					window = it->get_sptr();
					if (window && window->GetUid() == windowId)
					{
						_windows.erase(it);
						break;
					}
				}
			}
			window.reset();

			d.DestructData<uid::Uid>();
		}

		virtual void HandleWindowClosing(mem::sptr<evnt::Event> e)
		{
			win::WindowClosingEvent& closing_event = (win::WindowClosingEvent&)(*e);
			win::WindowClosingEvent::DataType& closing_data = closing_event.GetData();

			NP_ENGINE_ASSERT(closing_data.job->GetDelegate().GetData<mem::sptr<win::Window>>(), 
				"Owner of the window should have submitted it to the closing job");
			
			jsys::JobSystem& job_system = _services->GetJobSystem();
			mem::sptr<jsys::Job> adjust_job = job_system.CreateJob();
			adjust_job->GetDelegate().ConstructData<uid::Uid>(closing_data.windowId);
			adjust_job->GetDelegate().SetCallback(this, AdjustForWindowClosingCallback);
			jsys::Job::AddDependency(closing_data.job, adjust_job);
			job_system.SubmitJob(jsys::JobPriority::Higher, adjust_job);

			// our closing job must be submitted here
			job_system.SubmitJob(jsys::JobPriority::Normal, closing_data.job);
			closing_data.job.reset();
			e->SetHandled();
		}

		static void HandleWindowClosedCallback(void* caller, mem::Delegate& d)
		{
			((WindowLayer*)caller)->HandleWindowClosedProcedure(d);
		}

		void HandleWindowClosedProcedure(mem::Delegate& d)
		{
			// ownership of window is now resolved by giving it to the window layer for cleanup on main thread
			_windows_to_destroy.enqueue(d.GetData<mem::sptr<win::Window>>());
			d.DestructData<mem::sptr<win::Window>>();
			
			{
				Lock l(_windows_mutex);
				if (_windows.empty()) //this relies on the window closing adjust job
					_services->GetEventSubmitter().Submit(mem::create_sptr<ApplicationCloseEvent>(_services->GetAllocator()));
			}
		}

		virtual void HandleWindowClosed(mem::sptr<evnt::Event> e)
		{
			win::WindowClosedEvent& closed_event = (win::WindowClosedEvent&)(*e);
			win::WindowClosedEvent::DataType& closed_data = closed_event.GetData();
			jsys::JobSystem& job_system = _services->GetJobSystem();

			mem::sptr<jsys::Job> handle_job = job_system.CreateJob();
			// ownership of window is moving from the event to our job procedure
			handle_job->GetDelegate().ConstructData<mem::sptr<win::Window>>(closed_data.window);
			handle_job->GetDelegate().SetCallback(this, HandleWindowClosedCallback);
			job_system.SubmitJob(jsys::JobPriority::Higher, handle_job);

			e->SetHandled();
		}

		virtual void HandleEvent(mem::sptr<evnt::Event> e) override
		{
			switch (e->GetType())
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
		WindowLayer(mem::sptr<srvc::Services> services): Layer(services)
		{
			win::Window::Init(win::WindowDetailType::Glfw);
			win::Window::Init(win::WindowDetailType::Sdl);
		}

		virtual ~WindowLayer()
		{
			{
				Lock l(_windows_mutex);
				_windows.clear();
			}

			Cleanup();

			win::Window::Terminate(win::WindowDetailType::Glfw);
			win::Window::Terminate(win::WindowDetailType::Sdl);
		}

		virtual void RegisterWindow(mem::sptr<win::Window> window)
		{
			Lock l(_windows_mutex);
			_windows.emplace_back(window);
		}

		virtual void Update(tim::DblMilliseconds time_delta) override
		{
			win::Window::Update(win::WindowDetailType::Glfw);
			win::Window::Update(win::WindowDetailType::Sdl);

			mem::sptr<win::Window> window = nullptr;
			{
				Lock l(_windows_mutex);
				for (auto it = _windows.begin(); it != _windows.end(); it++)
				{
					window = it->get_sptr();
					if (window)
						window->Update(time_delta);
				}
			}
			window.reset();
		}

		virtual void Cleanup() override
		{
			mem::sptr<win::Window> window = nullptr;
			while (_windows_to_destroy.try_dequeue(window)) {}
			window.reset();

			{
				Lock l(_windows_mutex);
				for (siz i = _windows.size() - 1; i < _windows.size(); i--)
					if (_windows[i].is_expired())
						_windows.erase(_windows.begin() + i);
			}
		}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
