//
//  EventQueue.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/12/21.
//

#ifndef NP_ENGINE_EVENT_QUEUE_HPP
#define NP_ENGINE_EVENT_QUEUE_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"

namespace np
{
    namespace event
    {
        class EventQueue
        {
        public:
            using BufferedEventQueue = concurrency::vendor::moodycamel::ConcurrentQueue<event::Event*>;
            
        private:
            atm_bl _buffer_flag;
            memory::LinearAllocator _buffer1;
            memory::LinearAllocator _buffer2;
            BufferedEventQueue _buffer1_events;
            BufferedEventQueue _buffer2_events;
            
            /**
             getst the buffer based on our buffer flag
             */
            memory::LinearAllocator* GetBuffer()
            {
                return _buffer_flag.load(mo_acquire) ? &_buffer1 : &_buffer2;
            }
            
            /**
             gets the other buffer based on our buffer flag
             */
            memory::LinearAllocator* GetOtherBuffer()
            {
                return _buffer_flag.load(mo_acquire) ? &_buffer2 : &_buffer1;
            }
            
            /**
             gets the buffered event queue based on the given buffer
             */
            BufferedEventQueue* GetEventQueue(memory::LinearAllocator* buffer)
            {
                return buffer == &_buffer1 ? &_buffer1_events : &_buffer2_events;
            }
            
        public:
            
            /**
             constructor
             */
            EventQueue(memory::Block block, siz buffered_events_size = 100):
            _buffer_flag(true),
            _buffer1(memory::Block{block.ptr, memory::CalcAlignedSize(block.size/2)}),
            _buffer2(memory::Block
            {
                static_cast<ui8*>(block.ptr) + memory::CalcAlignedSize(block.size/2),
                memory::CalcAlignedSize(block.size/2)
            }),
            _buffer1_events(buffered_events_size),
            _buffer2_events(buffered_events_size)
            {}
            
            /**
             constructor
             */
            EventQueue(siz buffered_events_size = 100):
            _buffer_flag(true),
            _buffer1(memory::MEGABYTE_SIZE),
            _buffer2(memory::MEGABYTE_SIZE),
            _buffer1_events(buffered_events_size),
            _buffer2_events(buffered_events_size)
            {}
            
            /**
             deconstructor
             */
            ~EventQueue() = default;
            
            /**
             clears the other buffer
             */
            void ClearOtherBuffer()
            {
                memory::LinearAllocator* buffer = GetOtherBuffer();
                BufferedEventQueue* events = GetEventQueue(buffer);
                
                event::Event* e;
                NP_ASSERT(!events->try_dequeue(e), "we require the current buffer to be empty before clearing the buffer");
                
                buffer->DeallocateAll();
            }
            
            /**
             swap which buffer we emplace or pop
             */
            void SwapBuffers()
            {
                bl flag = _buffer_flag.load(mo_acquire);
                while(_buffer_flag.compare_exchange_weak(flag, !flag, mo_release, mo_relaxed));
            }
            
            /**
             emplace into the current buffer
             */
            template <typename T, typename ... Args>
            bl Emplace(Args&& ... args)
            {
                NP_STATIC_ASSERT((::std::is_base_of_v<event::Event, T>), "T is requried to be a base of event:Event");
                bl pushed = false;
                memory::LinearAllocator* buffer = GetBuffer();
                BufferedEventQueue* events = GetEventQueue(buffer);
                
                memory::Block block = buffer->Allocate(sizeof(T));
                if (block.IsValid())
                {
                    memory::Construct<T>(block, ::std::forward<Args>(args)...);
                    pushed = events->try_enqueue((event::Event*)block.ptr);
                }
                
                return pushed;
            }
            
            /**
             pop from the other buffer
             */
            event::Event* PopOther()
            {
                event::Event* e = nullptr;
                memory::LinearAllocator* buffer = GetOtherBuffer();
                BufferedEventQueue* events = GetEventQueue(buffer);
                
                if (!events->try_dequeue(e))
                {
                    e = nullptr;
                }
                
                return e;
            }
        };
    }
}

#endif /* NP_ENGINE_EVENT_QUEUE_HPP */
