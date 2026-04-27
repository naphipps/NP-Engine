//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WIN_WINDOW_EVENTS_HPP
#define NP_ENGINE_WIN_WINDOW_EVENTS_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Event/Event.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	template <typename DataType>
	class WindowEvent : public evnt::Event
	{
	protected:
		mem::trait_allocator _allocator;
		const evnt::EventType _type;

		WindowEvent(evnt::EventType type) :
			Event(),
			_type(evnt::EventType::Window | type.GetIntention() | type.GetTopic())
		{}

	public:
		virtual ~WindowEvent()
		{
			mem::destroy<DataType>(_allocator, static_cast<DataType*>(GetPayload()));
		}

		DataType& GetData()
		{
			return *static_cast<DataType*>(GetPayload());
		}

		const DataType& GetData() const
		{
			return *static_cast<DataType*>(GetPayload());
		}

		virtual evnt::EventType GetEventType() const override
		{
			return _type;
		}
	};

	struct WindowEventData
	{
		uid::Uid windowId{};
	};

	struct WindowCreateEventData : public WindowEventData
	{
		DetailType detailType = DetailType::None;
	};

	class WindowCreateEvent : public WindowEvent<WindowCreateEventData>
	{
	public:
		WindowCreateEvent(evnt::EventType intention, uid::Uid window_id, DetailType detail_type) :
			WindowEvent<WindowCreateEventData>(evnt::EventType::Create | intention.GetIntention())
		{
			SetPayload(mem::create<WindowCreateEventData>(_allocator, window_id, detail_type));
		}
	};

	struct WindowFocusEventData : public WindowEventData
	{
		bl isFocused = false;
	};

	class WindowFocusEvent : public WindowEvent<WindowFocusEventData>
	{
	public:
		WindowFocusEvent(evnt::EventType intention, uid::Uid window_id, bl is_focused) :
			WindowEvent<WindowFocusEventData>(evnt::EventType::Focus | intention.GetIntention())
		{
			SetPayload(mem::create<WindowFocusEventData>(_allocator, window_id, is_focused));
		}
	};

	struct WindowSizeEventData : public WindowEventData
	{
		::glm::uvec2 size{ 0 };
	};

	class WindowSizeEvent : public WindowEvent<WindowSizeEventData>
	{
	public:
		WindowSizeEvent(evnt::EventType intention, uid::Uid window_id, ::glm::uvec2 size) :
			WindowEvent<WindowSizeEventData>(evnt::EventType::Size | intention.GetIntention())
		{
			SetPayload(mem::create<WindowSizeEventData>(_allocator, window_id, size));
		}
	};

	struct WindowMinimizeEventData : public WindowEventData
	{
		bl isMinimized = false;
	};

	class WindowMinimizeEvent : public WindowEvent<WindowMinimizeEventData>
	{
	public:
		WindowMinimizeEvent(evnt::EventType intention, uid::Uid window_id, bl is_minimized) :
			WindowEvent<WindowMinimizeEventData>(evnt::EventType::Minimize | intention.GetIntention())
		{
			SetPayload(mem::create<WindowMinimizeEventData>(_allocator, window_id, is_minimized));
		}
	};

	struct WindowMaximizeEventData : public WindowEventData
	{
		bl isMaximized = false;
	};

	class WindowMaximizeEvent : public WindowEvent<WindowMaximizeEventData>
	{
	public:
		WindowMaximizeEvent(evnt::EventType intention, uid::Uid window_id, bl is_maximized) :
			WindowEvent<WindowMaximizeEventData>(evnt::EventType::Maximize | intention.GetIntention())
		{
			SetPayload(mem::create<WindowMaximizeEventData>(_allocator, window_id, is_maximized));
		}
	};

	struct WindowPositionEventData : public WindowEventData
	{
		::glm::ivec2 position{ 0 };
	};

	class WindowPositionEvent : public WindowEvent<WindowPositionEventData>
	{
	public:
		WindowPositionEvent(evnt::EventType intention, uid::Uid window_id, ::glm::ivec2 position) :
			WindowEvent<WindowPositionEventData>(evnt::EventType::Position | intention.GetIntention())
		{
			SetPayload(mem::create<WindowPositionEventData>(_allocator, window_id, position));
		}
	};

	struct WindowFramebufferSizeEventData : public WindowEventData
	{
		::glm::uvec2 size{ 0 };
	};

	class WindowFramebufferSizeEvent : public WindowEvent<WindowFramebufferSizeEventData>
	{
	public:
		WindowFramebufferSizeEvent(evnt::EventType intention, uid::Uid window_id, ::glm::uvec2 size) :
			WindowEvent<WindowFramebufferSizeEventData>(evnt::EventType::Framebuffer | evnt::EventType::Size | intention.GetIntention())
		{
			SetPayload(mem::create<WindowFramebufferSizeEventData>(_allocator, window_id, size));
		}
	};

	struct WindowTitleEventData : public WindowEventData
	{
		str title = "";
	};

	class WindowTitleEvent : public WindowEvent<WindowTitleEventData>
	{
	public:
		WindowTitleEvent(evnt::EventType intention, uid::Uid window_id, str title) :
			WindowEvent<WindowTitleEventData>(evnt::EventType::Title | intention.GetIntention())
		{
			SetPayload(mem::create<WindowTitleEventData>(_allocator, window_id, title));
		}
	};

	class WindowCloseEvent : public WindowEvent<WindowEventData>
	{
	public:
		WindowCloseEvent(evnt::EventType intention, uid::Uid window_id) :
			WindowEvent<WindowEventData>(evnt::EventType::Close | intention.GetIntention())
		{
			SetPayload(mem::create<WindowEventData>(_allocator, window_id));
		}
	};

	class WindowDestroyEvent : public WindowEvent<WindowEventData>
	{
	public:
		WindowDestroyEvent(evnt::EventType intention, uid::Uid window_id) :
			WindowEvent<WindowEventData>(evnt::EventType::Destroy | intention.GetIntention())
		{
			SetPayload(mem::create<WindowEventData>(_allocator, window_id));
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WIN_WINDOW_EVENTS_HPP */
