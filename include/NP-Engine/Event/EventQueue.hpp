//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_EVENT_QUEUE_HPP
#define NP_ENGINE_EVENT_QUEUE_HPP

#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "EventImpl.hpp"

namespace np::evnt
{
	class EventQueue
	{
	protected:
		using EventsQueue = mutexed_wrapper<con::queue<mem::sptr<Event>>>;
		atm_bl _flag;
		con::array<EventsQueue, 2> _events_queues;

		EventsQueue& GetQueue(bl flag)
		{
			return flag ? _events_queues.front() : _events_queues.back();
		}

	public:
		EventQueue(): _flag(true) {}

		void ToggleState()
		{
			bl flag = _flag.load(mo_acquire);
			while (!_flag.compare_exchange_weak(flag, !flag, mo_release, mo_relaxed))
			{}
		}

		void Push(mem::sptr<Event> e)
		{
			GetQueue(_flag.load(mo_acquire)).get_access()->emplace(e);
		}

		mem::sptr<Event> Pop()
		{
			mem::sptr<Event> e = nullptr;
			auto queue = GetQueue(!_flag.load(mo_acquire)).get_access();
			if (!queue->empty())
			{
				e = queue->front();
				queue->pop();
			}
			return e;
		}

		void Clear()
		{
			for (auto it = _events_queues.begin(); it != _events_queues.end(); it++)
			{
				auto queue = it->get_access();
				while (!queue->empty())
					queue->pop();
			}
		}
	};
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_QUEUE_HPP */
