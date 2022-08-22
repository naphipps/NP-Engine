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
			AssignData<DataType>({mem::AddressOf(window)});
		}

		event::EventType GetType() const override
		{
			return event::EventType::WindowClose;
		}

		event::EventCategory GetCategory() const override
		{
			return event::EventCategory::Window;
		}
	};
} // namespace np::window

#endif /* NP_ENGINE_WINDOW_CLOSE_EVENT_HPP */
