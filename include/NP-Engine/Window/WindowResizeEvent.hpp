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

namespace np::win
{
	class WindowResizeEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window* window;
			ui32 width;
			ui32 height;
		};

		WindowResizeEvent(Window& window, ui32 width, ui32 height): evnt::Event()
		{
			SetData<DataType>({mem::AddressOf(window), width, height});
		}

		i32 GetWidth() const
		{
			return GetData<DataType>().width;
		}

		i32 GetHeight() const
		{
			return GetData<DataType>().height;
		}

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowResize;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_RESIZE_EVENT_HPP */
