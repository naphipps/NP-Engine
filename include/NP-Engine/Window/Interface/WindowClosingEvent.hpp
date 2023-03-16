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
			uid::Uid windowId;
			mem::sptr<jsys::Job> job;
		};

		WindowClosingEvent(uid::Uid windowId, mem::sptr<jsys::Job> job): evnt::Event()
		{
			ConstructData<DataType>(DataType{ windowId, job });
		}

		~WindowClosingEvent()
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
			return evnt::EventType::WindowClosing;
		}

		evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_CLOSING_EVENT_HPP */