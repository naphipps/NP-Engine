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
	enum class EventType : ui64
	{
		None,

		WindowCreate,
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFocus,
		WindowMoved,

		KeyPressed,
		KeyReleased,
		KeyRepeated,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseScrolled,

		ApplicationClose,
		ApplicationPopup,

		GraphicsCreateRendererForWindow,
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
		Input = BIT(2),
		Key = BIT(3),
		Mouse = BIT(4),
		MouseButton = BIT(5) | Mouse,
		Graphics = BIT(6),
	};

	/*
		determines if EventCategory a contains b
	*/
	constexpr static bl EventCategoryContains(EventCategory a, EventCategory b)
	{
		return ((ui64)a & (ui64)b) != (ui64)EventCategory::None;
	}

	class Event : public memory::Delegate
	{
	protected:
		bl _handled;
		bl _can_be_handled; // TODO: refactor to _keep_alive?

	public:
		Event(): memory::Delegate(), _handled(false), _can_be_handled(false) {}

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
} // namespace np::event

#endif /* NP_ENGINE_EVENT_IMPL_HPP */
