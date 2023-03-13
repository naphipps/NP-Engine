//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_POSITION_EVENT_HPP
#define NP_ENGINE_WINDOW_POSITION_EVENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Event/Event.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	class WindowPositionEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window* window;
			i32 x;
			i32 y;
		};

		WindowPositionEvent(Window* window, i32 x, i32 y): evnt::Event()
		{
			SetData<DataType>({window, x, y});
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowPosition;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_POSITION_EVENT_HPP */