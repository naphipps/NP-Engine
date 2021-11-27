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
	class ApplicationPopupEvent : public event::Event
	{
	public:
		struct DataType
		{
			str message;
			Popup::Style style;
			Popup::Buttons buttons;
			Popup::Select select;
		};

		ApplicationPopupEvent(str message, Popup::Style style, Popup::Buttons buttons)
		{
			AssignData<DataType>({message, style, buttons, Popup::Select::None});
		}

		event::EventType GetType() const override
		{
			return event::EVENT_TYPE_APPLICATION_POPUP;
		}

		event::EventCategory GetCategory() const override
		{
			return event::EVENT_CATEGORY_APPLICATION;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_APPLICATION_POPUP_EVENT_HPP */
