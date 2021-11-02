//
//  ThreadPool.h
//  Project Space
//
//  Created by Nathan Phipps on 8/25/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_THREAD_POOL_HPP
#define NP_ENGINE_THREAD_POOL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "Thread.hpp"
#include "ThreadToken.hpp"

namespace np
{
    namespace concurrency
    {
        class ThreadPool : protected memory::ObjectPool<Thread>
        {
        private:
            using base = memory::ObjectPool<Thread>;
            
        public:
            constexpr static ui64 MAX_THREAD_COUNT = 24; //TODO: look like we can support more cores going to other machines
            
        public:
            
            /**
             constructor
             */
            ThreadPool():
            base(Thread::HardwareConcurrency() < MAX_THREAD_COUNT ? Thread::HardwareConcurrency() : MAX_THREAD_COUNT)
            {}
            
            /**
             passes through the ObjectPool's ObjectCount
             */
            siz ObjectCount() const
            {
                return base::ObjectCount();
            }
            
            /**
             passes through the ObjectPool's Clear
             */
            void Clear()
            {
                base::Clear();
            }
            
            /**
             creates a thread with the given function and args
             */
            template <class Function, class... Args>
            ThreadToken CreateThread(Function&& f, Args&&... args)
            {
                return CreateThread(-1, f, args...);
            }
            
            /**
             creates a thread with the given thread affinity, function, and args
             */
            template <class Function, class... Args>
            ThreadToken CreateThread(i32 thread_affinity, Function&& f, Args&&... args)
            {
                NP_PROFILE_FUNCTION();
                
                Thread* thread = base::CreateObject();
                thread->Run(f, args...);
                
                {
                    NP_PROFILE_SCOPE("attempt affinity");
                    if (thread_affinity > -1)
                    {
                        thread->SetAffinity(thread_affinity);
                    }
                }
                
                ThreadToken token(thread);
                return token;
            }
            
            /**
             removes the given thread by deallocating it
             */
            bl RemoveThread(ThreadToken token)
            {
                return DestroyObject(&token.GetThread());
            }
        };
    }
}

#endif /* NP_ENGINE_THREAD_POOL_HPP */
