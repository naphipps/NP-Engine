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

		ApplicationClose,
		ApplicationPopup,

		WindowCreate,
		WindowSetClose,
		WindowClosing,
		WindowFramebuffer,
		WindowSetTitle,
		WindowSetSize,
		WindowSize,
		WindowSetPosition,
		WindowPosition,
		WindowSetFocus,
		WindowFocus,
		WindowSetMinimize,
		WindowMinimize,
		WindowSetMaximize,
		WindowMaximize,

		NetworkClient,

		Max
	};

	class EventCategory : public Enum<ui64>
	{
	public:
		constexpr static ui64 Application = BIT(0);
		constexpr static ui64 Window = BIT(1);
		constexpr static ui64 Gpu = BIT(2);
		constexpr static ui64 Network = BIT(3);

		EventCategory(ui64 value) : Enum<ui64>(value) {}
	};

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
		virtual EventType GetType() const = 0; //TODO: rename to GetEventType

		/*
			get the category this event should be treated as
		*/
		virtual EventCategory GetCategory() const = 0; //TODO: rename to GetEventCategory
	};
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_IMPL_HPP */
