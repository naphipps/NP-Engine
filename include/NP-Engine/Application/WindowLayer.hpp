//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/16/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_APP_WINDOW_LAYER_HPP
#define NP_ENGINE_APP_WINDOW_LAYER_HPP

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "Layer.hpp"
#include "ApplicationEvents.hpp"

namespace np::app
{
	class WindowLayer : public Layer
	{
	private:
		const thr::thread::id _owning_thread_id;
		mutexed_wrapper<con::vector<mem::sptr<win::Window>>> _windows;
		mutexed_wrapper<con::uset<uid::Uid>> _to_destroy;
		evnt::EventQueue _deferred_event_queue{};

	protected:
		struct WindowClosingPayload
		{
			WindowLayer* caller = nullptr;
			uid::Uid windowId{};
		};

		void HandleWindowCreate(mem::sptr<evnt::Event> e)
		{
			mem::sptr<win::WindowCreateEvent> event = e;
			win::WindowCreateEventData& data = event->GetData();
			CreateWindow(data.detailType, data.windowId);
			// ^ submits another create event if this is not the owning thread, so considered handled
			e->SetIsHandled();
		}

		void HandleWindowTitle(mem::sptr<evnt::Event> e)
		{
			if (e->GetEventType().Contains(evnt::EventType::Will))
			{
				mem::sptr<win::WindowTitleEvent> event = e;
				win::WindowTitleEventData& data = event->GetData();

				auto windows = _windows.get_access();
				for (auto it = windows->begin(); it != windows->end(); it++)
					if ((*it)->GetUid() == data.windowId)
					{
						(*it)->SetTitle(data.title);
						break;
					}

				e->SetIsHandled();
			}
		}

		void HandleWindowFocus(mem::sptr<evnt::Event> e)
		{
			if (e->GetEventType().Contains(evnt::EventType::Will))
			{
				mem::sptr<win::WindowFocusEvent> event = e;
				win::WindowFocusEventData& data = event->GetData();

				if (data.isFocused)
				{
					auto windows = _windows.get_access();
					for (auto it = windows->begin(); it != windows->end(); it++)
						if ((*it)->GetUid() == data.windowId)
						{
							(*it)->Focus();
							break;
						}
				}

				e->SetIsHandled();
			}
		}

		void HandleWindowMaximize(mem::sptr<evnt::Event> e)
		{
			if (e->GetEventType().Contains(evnt::EventType::Will))
			{
				mem::sptr<win::WindowMaximizeEvent> event = e;
				win::WindowMaximizeEventData& data = event->GetData();

				auto windows = _windows.get_access();
				for (auto it = windows->begin(); it != windows->end(); it++)
					if ((*it)->GetUid() == data.windowId)
					{
						if (data.isMaximized)
							(*it)->Maximize();
						else
							(*it)->RestoreFromMaximize();
						break;
					}

				e->SetIsHandled();
			}
		}

		void HandleWindowMinimize(mem::sptr<evnt::Event> e)
		{
			if (e->GetEventType().Contains(evnt::EventType::Will))
			{
				mem::sptr<win::WindowMinimizeEvent> event = e;
				win::WindowMinimizeEventData& data = event->GetData();

				auto windows = _windows.get_access();
				for (auto it = windows->begin(); it != windows->end(); it++)
					if ((*it)->GetUid() == data.windowId)
					{
						if (data.isMinimized)
							(*it)->Minimize();
						else
							(*it)->RestoreFromMinimize();
						break;
					}

				e->SetIsHandled();
			}
		}

		void HandleWindowPosition(mem::sptr<evnt::Event> e)
		{
			if (e->GetEventType().Contains(evnt::EventType::Will))
			{
				mem::sptr<win::WindowPositionEvent> event = e;
				win::WindowPositionEventData& data = event->GetData();

				auto windows = _windows.get_access();
				for (auto it = windows->begin(); it != windows->end(); it++)
					if ((*it)->GetUid() == data.windowId)
					{
						(*it)->SetPosition(data.position);
						break;
					}

				e->SetIsHandled();
			}
		}

		void HandleWindowSize(mem::sptr<evnt::Event> e)
		{
			if (e->GetEventType().Contains(evnt::EventType::Will))
			{
				mem::sptr<win::WindowSizeEvent> event = e;
				win::WindowSizeEventData& data = event->GetData();

				auto windows = _windows.get_access();
				for (auto it = windows->begin(); it != windows->end(); it++)
					if ((*it)->GetUid() == data.windowId)
					{
						(*it)->SetSize(data.size);
						break;
					}

				e->SetIsHandled();
			}
		}

		void HandleWindowWillClose(mem::sptr<evnt::Event> e)
		{
			mem::sptr<win::WindowCloseEvent> event = e;
			win::WindowEventData& data = event->GetData();

			auto windows = _windows.get_access();
			for (auto it = windows->begin(); it != windows->end(); it++)
				if ((*it)->GetUid() == data.windowId)
				{
					(*it)->Close();
					break;
				}

			e->SetIsHandled();
		}

		void HandleWindowDidClose(mem::sptr<evnt::Event> e)
		{
			mem::sptr<win::WindowCloseEvent> event = e;
			win::WindowEventData& data = event->GetData();

			auto windows = _windows.get_access();
			for (auto it = windows->begin(); it != windows->end(); it++)
			{
				if ((*it)->GetUid() == data.windowId)
				{
					_to_destroy.get_access()->emplace(data.windowId);
					break;
				}
			}

			e->SetIsHandled();
		}

		void HandleWindowClose(mem::sptr<evnt::Event> e)
		{
			switch (e->GetEventType().GetIntention())
			{
			case evnt::EventType::Will:
				HandleWindowWillClose(e);
				break;

			case evnt::EventType::Did:
				HandleWindowDidClose(e);
				break;

			default:
				break;
			}
		}

		void HandleEvent(mem::sptr<evnt::Event> e) override
		{
			switch (e->GetEventType().GetTopic())
			{
			case evnt::EventType::Create:
				HandleWindowCreate(e);
				break;

			case evnt::EventType::Title:
				HandleWindowTitle(e);
				break;

			case evnt::EventType::Focus:
				HandleWindowFocus(e);
				break;

			case evnt::EventType::Maximize:
				HandleWindowMaximize(e);
				break;

			case evnt::EventType::Minimize:
				HandleWindowMinimize(e);
				break;

			case evnt::EventType::Position:
				HandleWindowPosition(e);
				break;

			case evnt::EventType::Size:
				HandleWindowSize(e);
				break;

			case evnt::EventType::Close:
				HandleWindowClose(e);
				break;

			default:
				break;
			}
		}

		bl IsOwningThread() const
		{
			return _owning_thread_id == thr::this_thread::get_id();
		}

	public:
		WindowLayer(mem::sptr<srvc::Services> services): Layer(services), _owning_thread_id(thr::this_thread::get_id())
		{
			win::Window::Init(win::DetailType::Glfw);
			win::Window::Init(win::DetailType::Sdl);
		}

		virtual ~WindowLayer()
		{
			_windows.get_access()->clear();
			_to_destroy.get_access()->clear();

			win::Window::Terminate(win::DetailType::Glfw);
			win::Window::Terminate(win::DetailType::Sdl);
		}

		mem::sptr<win::Window> CreateWindow(win::DetailType detail_type, uid::Uid id)
		{
			mem::sptr<win::Window> window = nullptr;
			if (IsOwningThread())
			{
				window = _windows.get_access()->emplace_back(win::Window::Create(detail_type, _services, id));
			}
			else
			{
				mem::sptr<evnt::Event> e = mem::create_sptr<win::WindowCreateEvent>(_services->GetAllocator(), evnt::EventType::Will, id, detail_type);
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

		virtual void OnEvent(mem::sptr<evnt::Event> e) override
		{
			if (CanHandle(e->GetEventType()))
			{
				if (IsOwningThread())
					HandleEvent(e);
				else
					_deferred_event_queue.Push(e);
			}
		}

		void BeforePoll() override
		{
			_deferred_event_queue.ToggleState();
			for (mem::sptr<evnt::Event> e = _deferred_event_queue.Pop(); e; e = _deferred_event_queue.Pop())
				HandleEvent(e); //we do not need to consider handled-ness or maintain these events for later handling
		}

		void Poll(tim::milliseconds time_delta) override
		{
			win::Window::Update(win::DetailType::Glfw);
			win::Window::Update(win::DetailType::Sdl);
		}

		void CleanupPoll() override
		{
			bl submit_application_close = false;
			{
				auto windows = _windows.get_access();
				for (auto wit = windows->begin(); wit != windows->end();)
				{
					if (*wit)
					{
						auto to_destroy = _to_destroy.get_access();
						auto dit = to_destroy->find((*wit)->GetUid());
						if (dit != to_destroy->end())
						{
							wit = windows->erase(wit);
							to_destroy->erase(dit);
							submit_application_close |= true;
						}
						else
						{
							wit++;
						}
					}
					else
					{
						wit = windows->erase(wit);
					}
				}

				submit_application_close &= windows->empty();
			}
			if (submit_application_close)
			{
				mem::sptr<evnt::Event> e =
					mem::create_sptr<ApplicationCloseEvent>(_services->GetAllocator(), evnt::EventType::Will, this);
				_services->GetEventSubmitter().Submit(e);
			}
		}

		virtual bl CanHandle(evnt::EventType type) const override
		{
			return type.Contains(evnt::EventType::Window);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_APP_WINDOW_LAYER_HPP */
