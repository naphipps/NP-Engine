//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/27/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_APPLICATION_POPUP_EVENT_HPP
#define NP_ENGINE_APPLICATION_POPUP_EVENT_HPP

#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Event/Event.hpp"

#include "Popup.hpp"

namespace np::app
{
	class ApplicationPopupEvent : public evnt::Event
	{
	protected:
		mem::trait_allocator _allocator;

	public:
		struct DataType
		{
			str message = "";;
			PopupStyle style = PopupStyle::None;
			PopupButtons buttons = PopupButtons::None;
			PopupSelection select = PopupSelection::None;
		};

		ApplicationPopupEvent(str message, PopupStyle style, PopupButtons buttons)
		{
			SetPayload(mem::create<DataType>(_allocator, message, style, buttons, PopupSelection::None));
		}

		~ApplicationPopupEvent()
		{
			mem::destroy<DataType>(_allocator, (DataType*)GetPayload());
		}

		DataType& GetData()
		{
			return *((DataType*)GetPayload());
		}

		const DataType& GetData() const
		{
			return *((DataType*)GetPayload());
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::ApplicationPopup;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Application;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_APPLICATION_POPUP_EVENT_HPP */
