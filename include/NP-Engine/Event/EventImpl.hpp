//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_EVENT_IMPL_HPP
#define NP_ENGINE_EVENT_IMPL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

namespace np::event
{
	using EventType = ui64;
	constexpr static EventType EVENT_TYPE_NONE = 0;
	constexpr static EventType EVENT_TYPE_WINDOW_CLOSE = 1;
	constexpr static EventType EVENT_TYPE_WINDOW_RESIZE = 2;
	constexpr static EventType EVENT_TYPE_WINDOW_FOCUS = 3;
	constexpr static EventType EVENT_TYPE_WINDOW_LOST_FOCUS = 4;
	constexpr static EventType EVENT_TYPE_WINDOW_MOVED = 5;
	constexpr static EventType EVENT_TYPE_KEY_PRESSED = 6;
	constexpr static EventType EVENT_TYPE_KEY_RELEASED = 7;
	constexpr static EventType EVENT_TYPE_KEY_REPREATED = 8;
	constexpr static EventType EVENT_TYPE_MOUSE_BUTTON_PRESSED = 9;
	constexpr static EventType EVENT_TYPE_MOUSE_BUTTON_RELEASED = 10;
	constexpr static EventType EVENT_TYPE_MOUSE_MOVED = 11;
	constexpr static EventType EVENT_TYPE_MOUSE_SCROLLED = 12;
	constexpr static EventType EVENT_TYPE_APPLICATION_CLOSE = 13;
	constexpr static EventType EVENT_TYPE_WINDOW_CREATE = 14;
	constexpr static EventType EVENT_TYPE_GRAPHICS_CREATE_RENDERER_FOR_WINDOW = 15;
	constexpr static EventType EVENT_TYPE_APPLICATION_POPUP = 16;

	/*
		determines if EventType a matches b
	*/
	constexpr static bl EventTypeMatches(EventType a, EventType b)
	{
		return a == b;
	}

	using EventCategory = ui64;
	constexpr static EventCategory EVENT_CATEGORY_NONE = 0;
	constexpr static EventCategory EVENT_CATEGORY_APPLICATION = BIT(0);
	constexpr static EventCategory EVENT_CATEGORY_WINDOW = BIT(1);
	constexpr static EventCategory EVENT_CATEGORY_INPUT = BIT(2);
	constexpr static EventCategory EVENT_CATEGORY_KEY = BIT(3);
	constexpr static EventCategory EVENT_CATEGORY_MOUSE = BIT(4);
	constexpr static EventCategory EVENT_CATEGORY_MOUSE_BUTTON = BIT(5) | EVENT_CATEGORY_MOUSE;
	constexpr static EventCategory EVENT_CATEGORY_GRAPHICS = BIT(6);

	/*
		determines if EventCategory a contains b
	*/
	constexpr static bl EventCategoryContains(EventCategory a, EventCategory b)
	{
		return (a & b) != EVENT_CATEGORY_NONE;
	}

	class Event : public memory::Delegate
	{
	protected:
		bl _handled;

	public:
		Event(): memory::Delegate(), _handled(false) {}

		virtual ~Event() {}

		void SetHandled(bl handled = true)
		{
			_handled = handled;
		}

		bl IsHandled() const
		{
			return _handled;
		}

		/*
			get the type that this event should be treated as
		*/
		virtual EventType GetType() const = 0;

		/*
			get the category this event should be treated as
		*/
		virtual EventCategory GetCategory() const = 0;
	};
} // namespace np::event

#endif /* NP_ENGINE_EVENT_IMPL_HPP */
