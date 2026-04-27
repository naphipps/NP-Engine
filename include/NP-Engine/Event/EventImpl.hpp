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
	class EventType : public enm_ui64
	{
	protected:

		constexpr static ui64 CategoryMask = BIT(10) - BIT(0);
		constexpr static ui64 IntentionMask = BIT(20) - BIT(10);
		constexpr static ui64 TopicMask = (BIT(63) - BIT(20)) | BIT(63);

	public:
		//category
		constexpr static ui64 Application = BIT(0);
		constexpr static ui64 Window = BIT(1);
		constexpr static ui64 Network = BIT(2);

		//intention
		constexpr static ui64 Will = BIT(10);
		constexpr static ui64 Did = BIT(11);
		
		//topic
		constexpr static ui64 Connect = BIT(51);
		constexpr static ui64 Server = BIT(52);
		constexpr static ui64 Client = BIT(53);
		constexpr static ui64 Popup = BIT(54);
		constexpr static ui64 Create = BIT(55);
		constexpr static ui64 Attention = BIT(56);
		constexpr static ui64 Focus = Attention;
		constexpr static ui64 Close = BIT(57);
		constexpr static ui64 Size = BIT(58);
		constexpr static ui64 Position = BIT(59);
		constexpr static ui64 Title = BIT(60);
		constexpr static ui64 Framebuffer = BIT(61);
		constexpr static ui64 Min = BIT(62);
		constexpr static ui64 Minimize = Min;
		constexpr static ui64 Max = BIT(63);
		constexpr static ui64 Maximize = Max;

		EventType(ui64 value) : enm_ui64(value) {}

		EventType GetCategory() const
		{
			return _value & CategoryMask;
		}

		EventType GetIntention() const
		{
			return _value & IntentionMask;
		}

		EventType GetTopic() const
		{
			return _value & TopicMask;
		}
	};

	class Event : public mem::delegate
	{
	protected:
		bl _handled;
		bl _can_be_handled;

	public:
		Event(): mem::delegate(), _handled(false), _can_be_handled(false) {}

		virtual ~Event() {}

		void SetIsHandled(bl handled = true)
		{
			_handled = handled;
		}

		bl IsHandled() const
		{
			return _handled;
		}

		void SetCanBeHandled(bl can_be_handled = true)
		{
			_can_be_handled = can_be_handled;
		}

		/*
			Note: handled events cannot be handled again
		*/
		bl CanBeHandled() const
		{
			return !IsHandled() && _can_be_handled;
		}

		virtual EventType GetEventType() const = 0;
	};
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_IMPL_HPP */
