//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_CLOSED_EVENT_HPP
#define NP_ENGINE_WINDOW_CLOSED_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	class WindowClosedEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window* window;
		};

		WindowClosedEvent(Window* window): evnt::Event()
		{
			SetData<DataType>({window});
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowClosed;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_CLOSED_EVENT_HPP */
