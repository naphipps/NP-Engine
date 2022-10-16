//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_FRAMEBUFFER_EVENT_HPP
#define NP_ENGINE_WINDOW_FRAMEBUFFER_EVENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Event/Event.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	class WindowFramebufferEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window* window;
			ui32 width;
			ui32 height;
		};

		WindowFramebufferEvent(Window* window, ui32 width, ui32 height) : evnt::Event()
		{
			SetData<DataType>({ window, width, height });
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowFramebuffer;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
}

#endif /* NP_ENGINE_WINDOW_FRAMEBUFFER_EVENT_HPP */