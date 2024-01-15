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
		mem::TraitAllocator _allocator;

		WindowEvent(): evnt::Event() {}

	public:
		virtual ~WindowEvent()
		{
			mem::Destroy<T>(_allocator, (T*)GetPayload());
		}

		T& GetData()
		{
			return *((T*)GetPayload());
		}

		const T& GetData() const
		{
			return *((T*)GetPayload());
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};

	// Window Create Events
	struct WindowCreateEventData
	{
		DetailType detailType;
		uid::Uid windowId;
	};

	class WindowCreateEvent : public WindowEvent<WindowCreateEventData>
	{
	public:
		WindowCreateEvent(DetailType detail_type, uid::Uid window_id): WindowEvent<WindowCreateEventData>()
		{
			SetPayload(mem::Create<WindowCreateEventData>(_allocator, detail_type, window_id));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowCreate;
		}
	};

	// Window Focus Events
	struct WindowFocusEventData
	{
		uid::Uid windowId;
		bl isFocused;
	};

	class WindowFocusEvent : public WindowEvent<WindowFocusEventData>
	{
	public:
		WindowFocusEvent(uid::Uid window_id, bl isFocused): WindowEvent<WindowFocusEventData>()
		{
			SetPayload(mem::Create<WindowFocusEventData>(_allocator, window_id, isFocused));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowFocus;
		}
	};

	class WindowSetFocusEvent : public WindowEvent<WindowFocusEventData>
	{
	public:
		WindowSetFocusEvent(uid::Uid window_id, bl isFocused): WindowEvent<WindowFocusEventData>()
		{
			SetPayload(mem::Create<WindowFocusEventData>(_allocator, window_id, isFocused));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetFocus;
		}
	};

	// Window Size Events
	struct WindowSizeEventData
	{
		uid::Uid windowId;
		ui32 width;
		ui32 height;
	};

	class WindowSizeEvent : public WindowEvent<WindowSizeEventData>
	{
	public:
		WindowSizeEvent(uid::Uid window_id, ui32 width, ui32 height): WindowEvent<WindowSizeEventData>()
		{
			SetPayload(mem::Create<WindowSizeEventData>(_allocator, window_id, width, height));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSize;
		}
	};

	class WindowSetSizeEvent : public WindowEvent<WindowSizeEventData>
	{
	public:
		WindowSetSizeEvent(uid::Uid window_id, ui32 width, ui32 height): WindowEvent<WindowSizeEventData>()
		{
			SetPayload(mem::Create<WindowSizeEventData>(_allocator, window_id, width, height));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetSize;
		}
	};

	// Window Minimize Events
	struct WindowMinimizeEventData
	{
		uid::Uid windowId;
		bl isMinimized;
	};

	class WindowMinimizeEvent : public WindowEvent<WindowMinimizeEventData>
	{
	public:
		WindowMinimizeEvent(uid::Uid window_id, bl isMinimized): WindowEvent<WindowMinimizeEventData>()
		{
			SetPayload(mem::Create<WindowMinimizeEventData>(_allocator, window_id, isMinimized));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowMinimize;
		}
	};

	class WindowSetMinimizeEvent : public WindowEvent<WindowMinimizeEventData>
	{
	public:
		WindowSetMinimizeEvent(uid::Uid window_id, bl isMinimized): WindowEvent<WindowMinimizeEventData>()
		{
			SetPayload(mem::Create<WindowMinimizeEventData>(_allocator, window_id, isMinimized));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetMinimize;
		}
	};

	// Window Maximize Events
	struct WindowMaximizeEventData
	{
		uid::Uid windowId;
		bl isMaximized;
	};

	class WindowMaximizeEvent : public WindowEvent<WindowMaximizeEventData>
	{
	public:
		WindowMaximizeEvent(uid::Uid window_id, bl isMaximized): WindowEvent<WindowMaximizeEventData>()
		{
			SetPayload(mem::Create<WindowMaximizeEventData>(_allocator, window_id, isMaximized));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowMaximize;
		}
	};

	class WindowSetMaximizeEvent : public WindowEvent<WindowMaximizeEventData>
	{
	public:
		WindowSetMaximizeEvent(uid::Uid window_id, bl isMaximized): WindowEvent<WindowMaximizeEventData>()
		{
			SetPayload(mem::Create<WindowMaximizeEventData>(_allocator, window_id, isMaximized));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetMaximize;
		}
	};

	// Window Position Events
	struct WindowPositionEventData
	{
		uid::Uid windowId;
		i32 x;
		i32 y;
	};

	class WindowPositionEvent : public WindowEvent<WindowPositionEventData>
	{
	public:
		WindowPositionEvent(uid::Uid window_id, i32 x, i32 y): WindowEvent<WindowPositionEventData>()
		{
			SetPayload(mem::Create<WindowPositionEventData>(_allocator, window_id, x, y));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowPosition;
		}
	};

	class WindowSetPositionEvent : public WindowEvent<WindowPositionEventData>
	{
	public:
		WindowSetPositionEvent(uid::Uid window_id, i32 x, i32 y): WindowEvent<WindowPositionEventData>()
		{
			SetPayload(mem::Create<WindowPositionEventData>(_allocator, window_id, x, y));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetPosition;
		}
	};

	// Window Framebuffer Events
	struct WindowFramebufferEventData
	{
		uid::Uid windowId;
		siz width;
		siz height;
	};

	class WindowFramebufferEvent : public WindowEvent<WindowFramebufferEventData>
	{
	public:
		WindowFramebufferEvent(uid::Uid window_id, ui32 width, ui32 height): WindowEvent<WindowFramebufferEventData>()
		{
			SetPayload(mem::Create<WindowFramebufferEventData>(_allocator, window_id, width, height));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowFramebuffer;
		}
	};

	// Window Title Events
	struct WindowTitleEventData
	{
		uid::Uid windowId;
		str title;
	};

	class WindowSetTitleEvent : public WindowEvent<WindowTitleEventData>
	{
	public:
		WindowSetTitleEvent(uid::Uid window_id, str title): WindowEvent<WindowTitleEventData>()
		{
			SetPayload(mem::Create<WindowTitleEventData>(_allocator, window_id, title));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetTitle;
		}
	};

	// Window Closed Events
	struct WindowCloseEventData
	{
		uid::Uid windowId;
	};

	class WindowSetCloseEvent : public WindowEvent<WindowCloseEventData>
	{
	public:
		WindowSetCloseEvent(uid::Uid window_id): WindowEvent<WindowCloseEventData>()
		{
			SetPayload(mem::Create<WindowCloseEventData>(_allocator, window_id));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowSetClose;
		}
	};

	// Window Closing Events
	struct WindowClosingEventData
	{
		uid::Uid windowId;
		mem::sptr<jsys::Job> job;
	};

	class WindowClosingEvent : public WindowEvent<WindowClosingEventData>
	{
	public:
		WindowClosingEvent(uid::Uid window_id, mem::sptr<jsys::Job> job): WindowEvent<WindowClosingEventData>()
		{
			SetPayload(mem::Create<WindowClosingEventData>(_allocator, window_id, job));
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowClosing;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_EVENTS_HPP */
