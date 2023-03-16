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

		WindowFramebufferEvent(Window* window, ui32 width, ui32 height): evnt::Event()
		{
			ConstructData<DataType>(DataType{ window, width, height });
		}

		~WindowFramebufferEvent()
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

		evnt::EventType GetType() const override
		{
			return evnt::EventType::WindowFramebuffer;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_FRAMEBUFFER_EVENT_HPP */