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
		const thr::Thread::Id _owning_thread_id;
		mutexed_wrapper<con::vector<mem::sptr<win::Window>>> _windows;
		mutexed_wrapper<con::uset<uid::Uid>> _windows_to_destroy;

	protected:
		struct WindowClosingPayload
		{
			WindowLayer* caller = nullptr;
			uid::Uid windowId{};
		};

		static void WindowClosedCallback(mem::Delegate& d)
		{
			WindowClosingPayload* payload = (WindowClosingPayload*)d.GetPayload();
			payload->caller->_windows_to_destroy.get_access()->emplace(payload->windowId);
			mem::Destroy<WindowClosingPayload>(payload->caller->_services->GetAllocator(), payload);
		}

		void HandleWindowClosing(mem::sptr<evnt::Event> e)
		{
			win::WindowClosingEvent& closing_event = (win::WindowClosingEvent&)(*e);
			win::WindowClosingEventData& closing_data = closing_event.GetData();
			jsys::JobSystem& job_system = _services->GetJobSystem();

			mem::sptr<jsys::Job> closed_job = job_system.CreateJob();
			closed_job->SetPayload(mem::Create<WindowClosingPayload>(_services->GetAllocator(), this, closing_data.windowId));
			closed_job->SetCallback(WindowClosedCallback);
			jsys::Job::AddDependency(closed_job, closing_data.job);

			job_system.SubmitJob(jsys::JobPriority::Higher, closing_data.job);
			job_system.SubmitJob(jsys::JobPriority::Normal, closed_job);

			e->SetHandled();
		}

		void HandleWindowSetTitle(mem::sptr<evnt::Event> e)
		{
			win::WindowSetTitleEvent& title_event = (win::WindowSetTitleEvent&)(*e);
			win::WindowTitleEventData& title_data = title_event.GetData();

			auto windows = _windows.get_access();
			for (auto it = windows->begin(); it != windows->end(); it++)
				if (*it && (*it)->GetUid() == title_data.windowId)
				{
					(*it)->SetTitle(title_data.title);
					break;
				}

			e->SetHandled();
		}

		void HandleWindowClose(mem::sptr<evnt::Event> e)
		{
			win::WindowSetCloseEvent& close_event = (win::WindowSetCloseEvent&)(*e);
			win::WindowCloseEventData& close_data = close_event.GetData();

			auto windows = _windows.get_access();
			for (auto it = windows->begin(); it != windows->end(); it++)
				if (*it && (*it)->GetUid() == close_data.windowId)
				{
					(*it)->Close();
					break;
				}

			e->SetHandled();
		}

		void HandleWindowSetFocus(mem::sptr<evnt::Event> e)
		{
			win::WindowSetFocusEvent& focus_event = (win::WindowSetFocusEvent&)(*e);
			win::WindowFocusEventData& focus_data = focus_event.GetData();

			if (focus_data.isFocused)
			{
				auto windows = _windows.get_access();
				for (auto it = windows->begin(); it != windows->end(); it++)
					if (*it && (*it)->GetUid() == focus_data.windowId)
					{
						(*it)->Focus();
						break;
					}
			}

			e->SetHandled();
		}

		void HandleWindowSetMaximize(mem::sptr<evnt::Event> e)
		{
			win::WindowSetMaximizeEvent& max_event = (win::WindowSetMaximizeEvent&)(*e);
			win::WindowMaximizeEventData& max_data = max_event.GetData();

			auto windows = _windows.get_access();
			for (auto it = windows->begin(); it != windows->end(); it++)
				if (*it && (*it)->GetUid() == max_data.windowId)
				{
					max_data.isMaximized ? (*it)->Maximize() : (*it)->RestoreFromMaximize();
					break;
				}

			e->SetHandled();
		}

		void HandleWindowSetMinimize(mem::sptr<evnt::Event> e)
		{
			win::WindowSetMinimizeEvent& min_event = (win::WindowSetMinimizeEvent&)(*e);
			win::WindowMinimizeEventData& min_data = min_event.GetData();

			auto windows = _windows.get_access();
			for (auto it = windows->begin(); it != windows->end(); it++)
				if (*it && (*it)->GetUid() == min_data.windowId)
				{
					min_data.isMinimized ? (*it)->Minimize() : (*it)->RestoreFromMinimize();
					break;
				}

			e->SetHandled();
		}

		void HandleWindowSetPosition(mem::sptr<evnt::Event> e)
		{
			win::WindowSetPositionEvent& position_event = (win::WindowSetPositionEvent&)(*e);
			win::WindowPositionEventData& position_data = position_event.GetData();

			auto windows = _windows.get_access();
			for (auto it = windows->begin(); it != windows->end(); it++)
				if (*it && (*it)->GetUid() == position_data.windowId)
				{
					(*it)->SetPosition(position_data.x, position_data.y);
					break;
				}

			e->SetHandled();
		}

		void HandleWindowSetSize(mem::sptr<evnt::Event> e)
		{
			win::WindowSetSizeEvent& size_event = (win::WindowSetSizeEvent&)(*e);
			win::WindowSizeEventData& size_data = size_event.GetData();

			auto windows = _windows.get_access();
			for (auto it = windows->begin(); it != windows->end(); it++)
				if (*it && (*it)->GetUid() == size_data.windowId)
				{
					(*it)->SetSize(size_data.width, size_data.height);
					break;
				}

			e->SetHandled();
		}

		void HandleWindowCreate(mem::sptr<evnt::Event> e)
		{
			win::WindowCreateEvent& create_event = (win::WindowCreateEvent&)(*e);
			win::WindowCreateEventData& create_data = create_event.GetData();
			Create(create_data.detailType, create_data.windowId);
			e->SetHandled();
		}

		void HandleEvent(mem::sptr<evnt::Event> e) override
		{
			switch (e->GetType())
			{
			case evnt::EventType::WindowClosing:
				HandleWindowClosing(e);
				break;

			case evnt::EventType::WindowSetTitle:
				HandleWindowSetTitle(e);
				break;

			case evnt::EventType::WindowSetClose:
				HandleWindowClose(e);
				break;

			case evnt::EventType::WindowSetFocus:
				HandleWindowSetFocus(e);
				break;

			case evnt::EventType::WindowSetMaximize:
				HandleWindowSetMaximize(e);
				break;

			case evnt::EventType::WindowSetMinimize:
				HandleWindowSetMinimize(e);
				break;

			case evnt::EventType::WindowSetPosition:
				HandleWindowSetPosition(e);
				break;

			case evnt::EventType::WindowSetSize:
				HandleWindowSetSize(e);
				break;

			case evnt::EventType::WindowCreate:
				HandleWindowCreate(e);
				break;

			default:
				break;
			}
		}

		bl IsOwningThread() const
		{
			return _owning_thread_id == thr::ThisThread::get_id();
		}

	public:
		WindowLayer(mem::sptr<srvc::Services> services): Layer(services), _owning_thread_id(thr::ThisThread::get_id())
		{
			win::Window::Init(win::DetailType::Glfw);
			win::Window::Init(win::DetailType::Sdl);
		}

		virtual ~WindowLayer()
		{
			_windows.get_access()->clear();
			_windows_to_destroy.get_access()->clear();

			win::Window::Terminate(win::DetailType::Glfw);
			win::Window::Terminate(win::DetailType::Sdl);
		}

		mem::sptr<win::Window> Create(win::DetailType detail_type, uid::Uid id)
		{
			mem::sptr<win::Window> window = nullptr;
			if (IsOwningThread())
			{
				window = _windows.get_access()->emplace_back(win::Window::Create(detail_type, _services, id));
			}
			else
			{
				mem::sptr<evnt::Event> e = mem::create_sptr<win::WindowCreateEvent>(_services->GetAllocator(), detail_type, id);
				_services->GetEventSubmitter().Submit(e);
			}
			return window;
		}

		mem::sptr<win::Window> Get(uid::Uid id)
		{
			mem::sptr<win::Window> window = nullptr;
			auto windows = _windows.get_access();
			for (auto it = windows->begin(); !window && it != windows->end(); it++)
				if ((*it)->GetUid() == id)
					window = *it;
			return window;
		}

		void Update(tim::DblMilliseconds time_delta) override
		{
			win::Window::Update(win::DetailType::Glfw);
			win::Window::Update(win::DetailType::Sdl);

			auto windows = _windows.get_access();
			for (auto it = windows->begin(); it != windows->end(); it++)
				if (*it)
					(*it)->Update(time_delta);
		}

		void Cleanup() override
		{
			bl submit_application_close = false;
			{
				auto windows = _windows.get_access();
				for (auto wit = windows->begin(); wit != windows->end();)
				{
					submit_application_close |= *wit;

					if (!*wit)
					{
						wit = windows->erase(wit);
						continue;
					}
					else if (*wit && wit->get_strong_count() == 1)
					{
						auto to_destroy = _windows_to_destroy.get_access();
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

		evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
