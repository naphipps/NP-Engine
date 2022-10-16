//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/22/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_CREATE_EVENT_HPP
#define NP_ENGINE_WINDOW_CREATE_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	class WindowCreateEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window::Properties window_properties;
		};

		WindowCreateEvent(const Window::Properties& window_properties): evnt::Event()
		{
			SetData<DataType>({window_properties});
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowCreate;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_CREATE_EVENT_HPP */
