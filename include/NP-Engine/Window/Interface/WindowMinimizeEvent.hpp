//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_MINIMIZE_EVENT_HPP
#define NP_ENGINE_WINDOW_MINIMIZE_EVENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Event/Event.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	class WindowMinimizeEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			Window* window;
			bl maximized;
		};

		WindowMinimizeEvent(Window* window, bl minimized): evnt::Event()
		{
			ConstructData<DataType>(DataType{ window, minimized });
		}

		~WindowMinimizeEvent()
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
			return evnt::EventType::WindowMinimize;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_MINIMIZE_EVENT_HPP */