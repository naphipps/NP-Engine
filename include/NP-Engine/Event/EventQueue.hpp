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
		using EventBufferType = con::mpmc_queue<mem::sptr<Event>>;
		//TODO: ^ we've had some issues where it takes too long for something to be pop after it has been pushed - maybe locking queue would be faster

		atm_bl _flag;
		mem::TraitAllocator _allocator;
		con::array<EventBufferType, 2> _buffers;

		EventBufferType& GetBuffer(bl flag)
		{
			return flag ? _buffers.front() : _buffers.back();
		}

	public:
		EventQueue(): _flag(true) {}

		void SwapBuffers()
		{
			bl flag = _flag.load(mo_acquire);
			while (!_flag.compare_exchange_weak(flag, !flag, mo_release, mo_relaxed)) {}
		}

		void Push(mem::sptr<Event> e)
		{
			GetBuffer(_flag.load(mo_acquire)).enqueue(e);
		}

		mem::sptr<Event> Pop()
		{
			mem::sptr<Event> e = nullptr;
			return GetBuffer(!_flag.load(mo_acquire)).try_dequeue(e) ? e : nullptr;
		}

		void Clear()
		{
			mem::sptr<Event> e = nullptr;
			for (auto it = _buffers.begin(); it != _buffers.end(); it++)
				while (it->try_dequeue(e)) {}

			e.reset();
		}
	};
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_QUEUE_HPP */
