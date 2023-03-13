//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_CLOSING_EVENT_HPP
#define NP_ENGINE_WINDOW_CLOSING_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/JobSystem/JobSystem.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	class WindowClosingEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window* window;
			jsys::Job* job;
		};

		WindowClosingEvent(Window* window, jsys::Job* job): evnt::Event()
		{
			SetData<DataType>({window, job});
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowClosing;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_CLOSING_EVENT_HPP */