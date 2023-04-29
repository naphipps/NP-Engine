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
		//TODO: move window ownership to window layer - set everything with events, and either use getters or events to get values
		using WindowsWrapper = mem::MutexedWrapper<con::vector<mem::sptr<win::Window>>>;
		using WindowsAccess = typename WindowsWrapper::Access;
		WindowsWrapper _windows;

		using WindowsToDestroyWrapper = mem::MutexedWrapper<con::uset<uid::Uid>>;
		using WindowsToDestroyAccess = typename WindowsToDestroyWrapper::Access;
		WindowsToDestroyWrapper _windows_to_destroy;

	protected:
		static void WindowClosedCallback(void* caller, mem::Delegate& d)
		{
			((WindowLayer*)caller)->WindowClosedProcedure(d);
		}

		void WindowClosedProcedure(mem::Delegate& d)
		{
			{
				_windows_to_destroy.GetAccess()->emplace(d.GetData<uid::Uid>());
			}
			d.DestructData<uid::Uid>();
		}

		virtual void HandleWindowClosing(mem::sptr<evnt::Event> e)
		{
			win::WindowClosingEvent& closing_event = (win::WindowClosingEvent&)(*e);
			win::WindowClosingEventData& closing_data = closing_event.GetData();
			jsys::JobSystem& job_system = _services->GetJobSystem();

			mem::sptr<jsys::Job> closed_job = job_system.CreateJob();
			closed_job->GetDelegate().ConstructData<uid::Uid>(closing_data.windowId);
			closed_job->GetDelegate().SetCallback(this, WindowClosedCallback);
			jsys::Job::AddDependency(closed_job, closing_data.job);

			job_system.SubmitJob(jsys::JobPriority::Higher, closing_data.job);
			job_system.SubmitJob(jsys::JobPriority::Normal, closed_job);

			e->SetHandled();
		}

		virtual void HandleWindowSetTitle(mem::sptr<evnt::Event> e)
		{
			win::WindowSetTitleEvent& title_event = (win::WindowSetTitleEvent&)(*e);
			win::WindowTitleEventData& title_data = title_event.GetData();

			{
				WindowsAccess windows = _windows.GetAccess();
				for (auto it = windows->begin(); it != windows->end(); it++)
					if (*it && (*it)->GetUid() == title_data.windowId)
					{
						(*it)->SetTitle(title_data.title);
						break;
					}
			}

			e->SetHandled();
		}

		virtual void HandleWindowClose(mem::sptr<evnt::Event> e)
		{
			win::WindowCloseEvent& close_event = (win::WindowCloseEvent&)(*e);
			win::WindowCloseEventData& close_data = close_event.GetData();

			{
				WindowsAccess windows = _windows.GetAccess();
				for (auto it = windows->begin(); it != windows->end(); it++)
					if (*it && (*it)->GetUid() == close_data.windowId)
					{
						(*it)->Close();
						break;
					}
			}

			e->SetHandled();
		}

		virtual void HandleEvent(mem::sptr<evnt::Event> e) override
		{
			switch (e->GetType())
			{
			case evnt::EventType::WindowClosing:
				HandleWindowClosing(e);
				break;

			case evnt::EventType::WindowSetTitle:
				HandleWindowSetTitle(e);
				break;

			case evnt::EventType::WindowClose:
				HandleWindowClose(e);
				break;

			case evnt::EventType::WindowSetFocus:
			case evnt::EventType::WindowSetMaximize:
			case evnt::EventType::WindowSetMinimize:
			case evnt::EventType::WindowSetPosition:
			case evnt::EventType::WindowSetSize:

			

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
            _windows.GetAccess()->clear();
            _windows_to_destroy.GetAccess()->clear();

			win::Window::Terminate(win::WindowDetailType::Glfw);
			win::Window::Terminate(win::WindowDetailType::Sdl);
		}

		virtual void RegisterWindow(mem::sptr<win::Window> window)
		{
			_windows.GetAccess()->emplace_back(window);
		}

		virtual void Update(tim::DblMilliseconds time_delta) override
		{
			win::Window::Update(win::WindowDetailType::Glfw);
			win::Window::Update(win::WindowDetailType::Sdl);

			{
				WindowsAccess windows = _windows.GetAccess();
				for (auto it = windows->begin(); it != windows->end(); it++)
					if (*it)
						(*it)->Update(time_delta);
			}
		}

		virtual void Cleanup() override
		{
            bl submit_application_close = false;
            {
                WindowsAccess windows = _windows.GetAccess();
                submit_application_close |= !windows->empty();
                
                for (auto wit = windows->begin(); wit != windows->end();)
                {
                    if (!*wit)
                    {
                        wit = windows->erase(wit);
                        continue;
                    }
                    else if (*wit && wit->get_strong_count() == 1)
                    {
                        WindowsToDestroyAccess to_destroy = _windows_to_destroy.GetAccess();
                        auto dit = to_destroy->find((*wit)->GetUid());
                        if (dit != to_destroy->end())
                        {
                            wit = windows->erase(wit);
                            to_destroy->erase(dit);
                            continue;
                        }
                    }
                    
                    wit++;
                }
                
                submit_application_close &= windows->empty();
            }
			if (submit_application_close)
				_services->GetEventSubmitter().Submit(mem::create_sptr<ApplicationCloseEvent>(_services->GetAllocator()));
		}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
