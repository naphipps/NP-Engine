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

namespace np::win
{
	class WindowCloseEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window* window;
		};

		WindowCloseEvent(Window& window): evnt::Event()
		{
			AssignData<DataType>({mem::AddressOf(window)});
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowClose;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_CLOSE_EVENT_HPP */
