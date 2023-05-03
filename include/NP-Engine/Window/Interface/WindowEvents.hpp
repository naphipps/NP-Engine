//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_EVENTS_HPP
#define NP_ENGINE_WINDOW_EVENTS_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Event/Event.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	template <typename T>
	class WindowEvent : public evnt::Event
	{
	protected:
		WindowEvent() : evnt::Event() {}

	public:

		virtual ~WindowEvent()
		{
			DestructData<T>();
		}

		T& GetData()
		{
			return mem::PadObject::GetData<T>();
		}

		const T& GetData() const
		{
			return mem::PadObject::GetData<T>();
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};

	//Window Focus Events
	struct WindowFocusEventData
	{
		uid::Uid windowId;
		bl isFocused;
	};

	class WindowFocusEvent : public WindowEvent<WindowFocusEventData>
	{
	public:
		WindowFocusEvent(uid::Uid windowId, bl isFocused) : WindowEvent<WindowFocusEventData>()
		{
			ConstructData<WindowFocusEventData>(WindowFocusEventData{ windowId, isFocused });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowFocus;
		}
	};

	class WindowSetFocusEvent : public WindowEvent<WindowFocusEventData>
	{
	public:
		WindowSetFocusEvent(uid::Uid windowId, bl isFocused) : WindowEvent<WindowFocusEventData>()
		{
			ConstructData<WindowFocusEventData>(WindowFocusEventData{ windowId, isFocused });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetFocus;
		}
	};

	//Window Size Events
	struct WindowSizeEventData
	{
		uid::Uid windowId;
		ui32 width;
		ui32 height;
	};

	class WindowSizeEvent : public WindowEvent<WindowSizeEventData>
	{
	public:

		WindowSizeEvent(uid::Uid windowId, ui32 width, ui32 height) : WindowEvent<WindowSizeEventData>()
		{
			ConstructData<WindowSizeEventData>(WindowSizeEventData{ windowId, width, height });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSize;
		}
	};

	class WindowSetSizeEvent : public WindowEvent<WindowSizeEventData>
	{
	public:

		WindowSetSizeEvent(uid::Uid windowId, ui32 width, ui32 height) : WindowEvent<WindowSizeEventData>()
		{
			ConstructData<WindowSizeEventData>(WindowSizeEventData{ windowId, width, height });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetSize;
		}
	};

	//Window Minimize Events
	struct WindowMinimizeEventData
	{
		uid::Uid windowId;
		bl isMinimized;
	};

	class WindowMinimizeEvent : public WindowEvent<WindowMinimizeEventData>
	{
	public:
		WindowMinimizeEvent(uid::Uid windowId, bl isMinimized) : WindowEvent<WindowMinimizeEventData>()
		{
			ConstructData<WindowMinimizeEventData>(WindowMinimizeEventData{ windowId, isMinimized });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowMinimize;
		}
	};

	class WindowSetMinimizeEvent : public WindowEvent<WindowMinimizeEventData>
	{
	public:
		WindowSetMinimizeEvent(uid::Uid windowId, bl isMinimized) : WindowEvent<WindowMinimizeEventData>()
		{
			ConstructData<WindowMinimizeEventData>(WindowMinimizeEventData{ windowId, isMinimized });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetMinimize;
		}
	};

	//Window Maximize Events
	struct WindowMaximizeEventData
	{
		uid::Uid windowId;
		bl isMaximized;
	};

	class WindowMaximizeEvent : public WindowEvent<WindowMaximizeEventData>
	{
	public:
		WindowMaximizeEvent(uid::Uid windowId, bl isMaximized) : WindowEvent<WindowMaximizeEventData>()
		{
			ConstructData<WindowMaximizeEventData>(WindowMaximizeEventData{ windowId, isMaximized });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowMaximize;
		}
	};

	class WindowSetMaximizeEvent : public WindowEvent<WindowMaximizeEventData>
	{
	public:
		WindowSetMaximizeEvent(uid::Uid windowId, bl isMaximized) : WindowEvent<WindowMaximizeEventData>()
		{
			ConstructData<WindowMaximizeEventData>(WindowMaximizeEventData{ windowId, isMaximized });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetMaximize;
		}
	};

	//Window Position Events
	struct WindowPositionEventData
	{
		uid::Uid windowId;
		i32 x;
		i32 y;
	};

	class WindowPositionEvent : public WindowEvent<WindowPositionEventData>
	{
	public:
		WindowPositionEvent(uid::Uid windowId, i32 x, i32 y) : WindowEvent<WindowPositionEventData>()
		{
			ConstructData<WindowPositionEventData>(WindowPositionEventData{ windowId, x, y });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowPosition;
		}
	};

	class WindowSetPositionEvent : public WindowEvent<WindowPositionEventData>
	{
	public:
		WindowSetPositionEvent(uid::Uid windowId, i32 x, i32 y) : WindowEvent<WindowPositionEventData>()
		{
			ConstructData<WindowPositionEventData>(WindowPositionEventData{ windowId, x, y });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetPosition;
		}
	};

	//Window Framebuffer Events
	struct WindowFramebufferEventData
	{
		uid::Uid windowId;
		siz width;
		siz height;
	};

	class WindowFramebufferEvent : public WindowEvent<WindowFramebufferEventData>
	{
	public:
		WindowFramebufferEvent(uid::Uid windowId, ui32 width, ui32 height) : WindowEvent<WindowFramebufferEventData>()
		{
			ConstructData<WindowFramebufferEventData>(WindowFramebufferEventData{ windowId, width, height });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowFramebuffer;
		}
	};

	//Window Title Events
	struct WindowTitleEventData
	{
		uid::Uid windowId;
		str title;
	};

	class WindowSetTitleEvent : public WindowEvent<WindowTitleEventData>
	{
	public:
		WindowSetTitleEvent(uid::Uid windowId, str title) : WindowEvent<WindowTitleEventData>()
		{
			ConstructData<WindowTitleEventData>(WindowTitleEventData{ windowId, title });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetTitle;
		}
	};
	
	//Window Closed Events
	struct WindowCloseEventData
	{
		uid::Uid windowId;
	};

	class WindowSetCloseEvent : public WindowEvent<WindowCloseEventData>
	{
	public:
		WindowSetCloseEvent(uid::Uid windowId) : WindowEvent<WindowCloseEventData>()
		{
			ConstructData<WindowCloseEventData>(WindowCloseEventData{ windowId });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetClose;
		}
	};

	//Window Closing Events
	struct WindowClosingEventData
	{
		uid::Uid windowId;
		mem::sptr<jsys::Job> job;
	};

	class WindowClosingEvent : public WindowEvent<WindowClosingEventData>
	{
	public:
		WindowClosingEvent(uid::Uid windowId, mem::sptr<jsys::Job> job) : WindowEvent<WindowClosingEventData>()
		{
			ConstructData<WindowClosingEventData>(WindowClosingEventData{ windowId, job });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowClosing;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_EVENTS_HPP */
