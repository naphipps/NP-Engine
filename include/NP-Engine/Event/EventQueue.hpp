//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_EVENT_QUEUE_HPP
#define NP_ENGINE_EVENT_QUEUE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"

#include "EventImpl.hpp"

// TODO: update comments

namespace np
{
	namespace event
	{
		class EventQueue
		{
		protected:
			atm_bl _flag;
			memory::TraitAllocator _allocator;
			concurrency::MpmcQueue<Event*> _buffer1;
			concurrency::MpmcQueue<Event*> _buffer2;

			/**
			 gets the event queue based on the queue flag
			 */
			concurrency::MpmcQueue<Event*>& GetBuffer()
			{
				return _flag.load(mo_acquire) ? _buffer1 : _buffer2;
			}

			/**
			 gets the other event queue based on the queue flag
			 */
			concurrency::MpmcQueue<Event*>& GetOtherBuffer()
			{
				return _flag.load(mo_acquire) ? _buffer2 : _buffer1;
			}

		public:
			/**
			 constructor
			 */
			EventQueue(siz buffer_size = 100): _flag(true), _buffer1(buffer_size), _buffer2(buffer_size) {}

			/**
			 swap which queue we emplace or pop
			 */
			void SwapBuffers()
			{
				bl flag = _flag.load(mo_acquire);
				while (_flag.compare_exchange_weak(flag, !flag, mo_release, mo_relaxed))
					;
			}

			/**
			 emplace into the current buffer
			 */
			template <typename T, typename... Args>
			bl Emplace(Args&&... args)
			{
				NP_STATIC_ASSERT((::std::is_base_of_v<event::Event, T>), "T is requried to be a base of event:Event");
				Event* event = nullptr;
				concurrency::MpmcQueue<Event*>& buffer = GetBuffer();

				memory::Block block = _allocator.Allocate(sizeof(T));
				if (block.IsValid()) // TODO: we need to mark checks like this to remove their else blocks
				{
					if (memory::Construct<T>(block, ::std::forward<Args>(args)...))
					{
						event = static_cast<Event*>(block.Begin());
						if (!buffer.enqueue(event))
						{
							memory::Destruct(static_cast<T*>(event));
							_allocator.Deallocate(event);
							event = nullptr;
						}
					}
					else
					{
						_allocator.Deallocate(block);
					}
				}

				return event != nullptr;
			}

			bl Emplace(const Event* event)
			{
				return GetBuffer().enqueue(const_cast<Event*>(event));
			}

			/**
			 pop from the other buffer
			 */
			event::Event* PopOther()
			{
				concurrency::MpmcQueue<Event*>& buffer = GetOtherBuffer();
				event::Event* e = nullptr;
				if (!buffer.try_dequeue(e))
				{
					e = nullptr;
				}

				return e;
			}

			bl DestroyEvent(Event* event)
			{
				return memory::Destruct(event) && _allocator.Deallocate(event);
			}
		};
	} // namespace event
} // namespace np

#endif /* NP_ENGINE_EVENT_QUEUE_HPP */
