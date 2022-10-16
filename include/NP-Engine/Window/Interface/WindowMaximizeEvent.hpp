//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_MAXIMIZE_EVENT_HPP
#define NP_ENGINE_WINDOW_MAXIMIZE_EVENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Event/Event.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	class WindowMaximizeEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window* window;
			bl maximized;
		};

		WindowMaximizeEvent(Window* window, bl maximized) : evnt::Event()
		{
			SetData<DataType>({ window, maximized });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowMaximize;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
}

#endif /* NP_ENGINE_WINDOW_MAXIMIZE_EVENT_HPP */