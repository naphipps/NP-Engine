//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_RESIZE_EVENT_HPP
#define NP_ENGINE_WINDOW_RESIZE_EVENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Event/Event.hpp"

#include "Window.hpp"

namespace np::window
{
	class WindowResizeEvent : public event::Event
	{
	public:
		struct DataType
		{
			Window* window;
			ui32 width;
			ui32 height;
		};

		WindowResizeEvent(Window& window, ui32 width, ui32 height): event::Event()
		{
			AssignData<DataType>({memory::AddressOf(window), width, height});
		}

		i32 GetWidth() const
		{
			return RetrieveData<DataType>().width;
		}

		i32 GetHeight() const
		{
			return RetrieveData<DataType>().height;
		}

		event::EventType GetType() const override
		{
			return event::EventType::WindowResize;
		}

		event::EventCategory GetCategory() const override
		{
			return event::EventCategory::Window;
		}
	};
} // namespace np::window

#endif /* NP_ENGINE_WINDOW_RESIZE_EVENT_HPP */
