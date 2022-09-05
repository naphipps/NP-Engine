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
		con::mpmc_queue<Event*> _buffer1;
		con::mpmc_queue<Event*> _buffer2;

		/*
			gets the event queue based on the queue flag
		*/
		con::mpmc_queue<Event*>& GetBuffer()
		{
			return _flag.load(mo_acquire) ? _buffer1 : _buffer2;
		}

		/*
			gets the other event queue based on the queue flag
		*/
		con::mpmc_queue<Event*>& GetOtherBuffer()
		{
			return _flag.load(mo_acquire) ? _buffer2 : _buffer1;
		}

	public:
		EventQueue(siz buffer_size = 100): _flag(true), _buffer1(buffer_size), _buffer2(buffer_size) {}

		void SwapBuffers()
		{
			bl flag = _flag.load(mo_acquire);
			while (_flag.compare_exchange_weak(flag, !flag, mo_release, mo_relaxed)) {}
		}

		/*
			emplace into the current buffer
		*/
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
			return GetBuffer().enqueue(e);
		}

		/*
			pop from the other buffer
		*/
		Event* PopOther()
		{
			Event* e = nullptr;
			return GetOtherBuffer().try_dequeue(e) ? e : nullptr;
		}

		void DestroyEvent(Event* e)
		{
			mem::Destroy<Event>(_allocator, e);
		}

		void Clear()
		{
			Event* e = nullptr;
			while (_buffer1.try_dequeue(e))
				DestroyEvent(e);

			while (_buffer2.try_dequeue(e))
				DestroyEvent(e);
		}
	};
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_QUEUE_HPP */
