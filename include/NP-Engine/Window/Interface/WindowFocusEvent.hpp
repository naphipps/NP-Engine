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
			uid::Uid windowId;
			bl focused;
		};

		WindowFocusEvent(uid::Uid windowId, bl focused): evnt::Event()
		{
			ConstructData<DataType>(DataType{ windowId, focused });
		}

		~WindowFocusEvent()
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
			return evnt::EventType::WindowFocus;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_FOCUS_EVENT_HPP */