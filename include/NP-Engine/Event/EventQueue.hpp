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
		container::mpmc_queue<Event*> _buffer1;
		container::mpmc_queue<Event*> _buffer2;

		/*
			gets the event queue based on the queue flag
		*/
		container::mpmc_queue<Event*>& GetBuffer()
		{
			return _flag.load(mo_acquire) ? _buffer1 : _buffer2;
		}

		/*
			gets the other event queue based on the queue flag
		*/
		container::mpmc_queue<Event*>& GetOtherBuffer()
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
			NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<evnt::Event, T>), "T is requried to be a base of event:Event");
			Event* e = nullptr;
			container::mpmc_queue<Event*>& buffer = GetBuffer();

			mem::Block block = _allocator.Allocate(sizeof(T));
			if (block.IsValid())
			{
				if (mem::Construct<T>(block, ::std::forward<Args>(args)...))
				{
					e = static_cast<Event*>(block.ptr);
					if (!buffer.enqueue(e))
					{
						mem::Destruct(static_cast<T*>(e));
						_allocator.Deallocate(e);
						e = nullptr;
					}
				}
				else
				{
					_allocator.Deallocate(block);
				}
			}

			return e != nullptr;
		}

		bl Emplace(const Event* e)
		{
			return GetBuffer().enqueue(const_cast<Event*>(e));
		}

		/*
			pop from the other buffer
		*/
		evnt::Event* PopOther()
		{
			container::mpmc_queue<Event*>& buffer = GetOtherBuffer();
			evnt::Event* e = nullptr;
			if (!buffer.try_dequeue(e))
			{
				e = nullptr;
			}

			return e;
		}

		bl DestroyEvent(Event* e)
		{
			bl destructed = mem::Destruct(e);
			bl deallocated = _allocator.Deallocate(e);
			return destructed && deallocated;
		}
	};
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_QUEUE_HPP */
