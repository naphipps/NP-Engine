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

#include "WindowImpl.hpp"

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

		WindowResizeEvent(Window* window, ui32 width, ui32 height): evnt::Event()
		{
			ConstructData<DataType>(DataType{ window, width, height });
		}

		~WindowResizeEvent()
		{
			DestructData<DataType>();
		}

		DataType& GetData()
		{
			return mem::PadObject::GetData<DataType>();
		}

		const DataType& GetData() const
		{
			return mem::PadObject::GetData<DataType>();
		}

		i32 GetWidth() const
		{
			return GetData().width;
		}

		i32 GetHeight() const
		{
			return GetData().height;
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
