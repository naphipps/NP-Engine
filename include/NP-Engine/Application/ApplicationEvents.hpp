//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/27/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_APP_APPLICATION_EVENTS_HPP
#define NP_ENGINE_APP_APPLICATION_EVENTS_HPP

#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "Popup.hpp"

namespace np::app
{
	template <typename DataType>
	class ApplicationEvent : public evnt::Event
	{
	protected:
		mem::trait_allocator _allocator;
		const evnt::EventType _type;

		ApplicationEvent(evnt::EventType type) :
			Event(),
			_type(evnt::EventType::Application | type.GetIntention() | type.GetTopic())
		{}

	public:
		virtual ~ApplicationEvent()
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

	struct ApplicationEventData
	{
		/*
			currently no use for this - value can be anything we don't care about
		*/
		void* ptr = nullptr;
	};

	struct ApplicationPopupEventData : public ApplicationEventData
	{
		str message = "";;
		PopupStyle style = PopupStyle::None;
		PopupButtons buttons = PopupButtons::None;
		PopupSelection select = PopupSelection::None;
	};

	class ApplicationPopupEvent : public ApplicationEvent<ApplicationPopupEventData>
	{
	public:
		ApplicationPopupEvent(evnt::EventType intention, void* ptr, str message, PopupStyle style, PopupButtons buttons):
			ApplicationEvent<ApplicationPopupEventData>(evnt::EventType::Popup | intention.GetIntention())
		{
			SetPayload(mem::create<ApplicationPopupEventData>(_allocator, ptr, message, style, buttons, PopupSelection::None));
		}
	};

	class ApplicationCloseEvent : public ApplicationEvent<ApplicationEventData>
	{
	public:
		ApplicationCloseEvent(evnt::EventType intention, void* ptr):
			ApplicationEvent<ApplicationEventData>(evnt::EventType::Close | intention.GetIntention())
		{
			SetPayload(mem::create<ApplicationEventData>(_allocator, ptr));
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_APP_APPLICATION_EVENTS_HPP */
