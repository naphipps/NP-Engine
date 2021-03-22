//
//  Thread.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/25/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_THREAD_HPP
#define NP_ENGINE_THREAD_HPP

#include <thread>

#include "Primitive/Primitive.hpp"

namespace np
{
    namespace concurrency
    {
        /**
         represent and wraps a std thread
         */
        class Thread
        {    
        public:
            using Id = ::std::thread::id;
            
            /**
             gets the hardware concurrency
             */
            static inline ui32 HardwareConcurrency() noexcept
            {
                return ::std::thread::hardware_concurrency();
            }
            
        private:
            bl _running;
            ui8 _thread_allocation[sizeof(::std::thread)];
            
            /**
             safely disposed of std Thread
             */
            void Dispose()
            {
                if (IsRunning())
                {
                    //TODO: do we need to handle affinity here??
                    
                    //deallocate std::Thread -- should always be joinable here
                    ::std::thread* std_thread = GetStdThreadPtr();
                    std_thread->join();
                    std_thread->::std::thread::~thread();
                    std_thread = NULL;
                    _running = false;
                }
            }
            
            /**
             gets the std thread ptr from our allocation
             */
            ::std::thread* GetStdThreadPtr()
            {
                return (::std::thread*)_thread_allocation;
            }
            
            /**
             gets the std thread ptr from our allocation
             */
            ::std::thread* GetStdThreadPtr() const
            {
                return (::std::thread*)_thread_allocation;
            }
            
        public:
            
            /**
             constructor
             */
            Thread():
            _running(false)
            {}
            
            /**
             deconstructor
             */
            ~Thread()
            {
                Dispose();
            }
            
            /**
             starts our Thread instance with the given function and args
             */
            template <class Function, class... Args>
            void Run(Function&& f, Args&&... args)
            {
                _running = true;
                new (_thread_allocation) ::std::thread(f, args...);
            }
            
            /**
             sets the Thread affinity given the core number
             */
            bl SetAffinity(const ui8 core_number)
            {
                //TODO: support affinity for all main OS's, then consoles if they support?? Switch, Xbox, PlayStation
                return false;
            }
            
            /**
             checks if our Thread is running or not
             */
            bl IsRunning() const
            {
                return _running;
            }
            
            /**
             get our inner std thread's id
             when our inner std thread is null, then we return a zeroed thread id
             */
            Id GetId() const
            {
                ::std::thread* std_thread = GetStdThreadPtr();
                return std_thread != NULL ? std_thread->get_id() : Id();
            }
        };
        
        /**
         represents std this_thread
         */
        namespace ThisThread
        {
            using namespace ::std::this_thread;
        }
    }
}

#endif /* NP_ENGINE_THREAD_HPP */
