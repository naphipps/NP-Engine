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

		ui32 GetWidth() const
		{
			return RetrieveData<DataType>().width;
		}

		ui32 GetHeight() const
		{
			return RetrieveData<DataType>().height;
		}

		event::EventType GetType() const override
		{
			return event::EVENT_TYPE_WINDOW_RESIZE;
		}

		event::EventCategory GetCategory() const override
		{
			return event::EVENT_CATEGORY_WINDOW;
		}
	};
} // namespace np::window

#endif /* NP_ENGINE_WINDOW_RESIZE_EVENT_HPP */
