//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_CLOSED_EVENT_HPP
#define NP_ENGINE_WINDOW_CLOSED_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "WindowImpl.hpp"

namespace np::win
{
	class WindowClosedEvent : public evnt::Event
	{
	public:
		struct DataType
		{
			mem::sptr<Window> window;
		};

		WindowClosedEvent(mem::sptr<Window> window): evnt::Event()
		{
			ConstructData<DataType>(DataType{ window });
		}

		~WindowClosedEvent()
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
			return evnt::EventType::WindowClosed;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_CLOSED_EVENT_HPP */
