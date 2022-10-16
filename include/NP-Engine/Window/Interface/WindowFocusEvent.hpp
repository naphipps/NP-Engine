//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_FOCUS_EVENT_HPP
#define NP_ENGINE_WINDOW_FOCUS_EVENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Event/Event.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	class WindowFocusEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window* window;
			bl focused;
		};

		WindowFocusEvent(Window* window, bl focused) : evnt::Event()
		{
			SetData<DataType>({ window, focused });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowFocus;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
}

#endif /* NP_ENGINE_WINDOW_FOCUS_EVENT_HPP */