//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_EVENT_IMPL_HPP
#define NP_ENGINE_EVENT_IMPL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

namespace np::evnt
{
	enum class EventType : ui64
	{
		None,

		WindowClosing,
		WindowClosed,
		WindowResize,
		WindowFocus,
		WindowPosition,
		WindowFramebuffer,
		WindowMinimize,
		WindowMaximize,

		ApplicationClose,
		ApplicationPopup,

		Max
	};

	/*
		determines if EventType a matches b
	*/
	constexpr static bl EventTypeMatches(EventType a, EventType b)
	{
		return a == b;
	}

	enum class EventCategory : ui64
	{
		None = 0,
		Application = BIT(0),
		Window = BIT(1),
		Graphics = BIT(2), //TODO: after we refactor our gfx namespace to gpu, refactor this to "Gpu"
		All = ~((ui64)0)
	};

	/*
		determines if EventCategory a contains b
	*/
	constexpr static bl EventCategoryContains(EventCategory a, EventCategory b)
	{
		return ((ui64)a & (ui64)b) != (ui64)EventCategory::None;
	}

	class Event : public mem::Delegate
	{
	protected:
		bl _handled;
		bl _can_be_handled;

	public:
		Event(): mem::Delegate(), _handled(false), _can_be_handled(false) {}

		virtual ~Event() {}

		void SetHandled(bl handled = true)
		{
			_handled = handled;
		}

		bl IsHandled() const
		{
			return _handled;
		}

		void SetCanBeHandled(bl can_be_handled = true)
		{
			_can_be_handled = true;
		}

		/*
			Note: handled events cannot be handled again
		*/
		bl CanBeHandled() const
		{
			return !IsHandled() && _can_be_handled;
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
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_IMPL_HPP */
