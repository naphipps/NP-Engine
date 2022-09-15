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
		atm_bl _flag;
		mem::TraitAllocator _allocator;
		con::array<con::mpmc_queue<Event*>, 2> _buffers;

		con::mpmc_queue<Event*>& GetBuffer(bl flag)
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

		template <typename T, typename... Args>
		bl Emplace(Args&&... args)
		{
			NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<Event, T>), "T is requried to be a base of event:Event");
			T* e = mem::Create<T>(_allocator, ::std::forward<Args>(args)...);

			if (e)
				if (!Emplace(e))
				{
					mem::Destroy<T>(_allocator, e);
					e = nullptr;
				}

			return e;
		}

		bl Emplace(Event* e)
		{
			return GetBuffer(_flag.load(mo_acquire)).enqueue(e);
		}

		Event* Pop()
		{
			Event* e = nullptr;
			return GetBuffer(!_flag.load(mo_acquire)).try_dequeue(e) ? e : nullptr;
		}

		void DestroyEvent(Event* e)
		{
			mem::Destroy<Event>(_allocator, e);
		}

		void Clear()
		{
			Event* e = nullptr;
			for (auto it = _buffers.begin(); it != _buffers.end(); it++)
				while (it->try_dequeue(e))
					DestroyEvent(e);
		}
	};
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_QUEUE_HPP */
