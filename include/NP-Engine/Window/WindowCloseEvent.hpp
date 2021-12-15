//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_CLOSE_EVENT_HPP
#define NP_ENGINE_WINDOW_CLOSE_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "Window.hpp"

namespace np::window
{
	class WindowCloseEvent : public event::Event
	{
	public:
		struct DataType
		{
			Window* window;
		};

		WindowCloseEvent(Window& window): event::Event()
		{
			AssignData<DataType>({memory::AddressOf(window)});
		}

		event::EventType GetType() const override
		{
			return event::EVENT_TYPE_WINDOW_CLOSE;
		}

		event::EventCategory GetCategory() const override
		{
			return event::EVENT_CATEGORY_WINDOW;
		}
	};
} // namespace np::window

#endif /* NP_ENGINE_WINDOW_CLOSE_EVENT_HPP */
