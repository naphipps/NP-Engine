//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/22/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_CREATE_EVENT_HPP
#define NP_ENGINE_WINDOW_CREATE_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"

#include "Window.hpp"

namespace np::window
{
	class WindowCreateEvent : public event::Event
	{
	public:
		struct DataType
		{
			Window::Properties window_properties;
		};

		WindowCreateEvent(Window::Properties window_properties): event::Event()
		{
			AssignData<DataType>({window_properties});
		}

		event::EventType GetType() const override
		{
			return event::EVENT_TYPE_WINDOW_CREATE;
		}

		event::EventCategory GetCategory() const override
		{
			return event::EVENT_CATEGORY_WINDOW;
		}
	};
} // namespace np::window

#endif /* NP_ENGINE_WINDOW_CREATE_EVENT_HPP */
