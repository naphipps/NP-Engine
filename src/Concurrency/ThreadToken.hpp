//
//  ThreadToken.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/27/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_THREAD_TOKEN_HPP
#define NP_ENGINE_THREAD_TOKEN_HPP

#include "Primitive/Primitive.hpp"

#include "Thread.hpp"

namespace np
{
    namespace concurrency
    {
        /**
         contains a Thread pointer so outside objects cannot manipulate it
         */
        class ThreadToken
        {
        private:
            Thread* _thread;
            
        public:
            
            /**
             constructor
             */
            ThreadToken():
            _thread(NULL)
            {}
            
            /**
             constructor - setting our Thread pointer to given Thread pointer
             */
            ThreadToken(Thread* thread):
            _thread(thread)
            {}
            
            /**
             copy constructor
             */
            ThreadToken(const ThreadToken& token):
            _thread(token._thread)
            {}
            
            /**
             move constructor - acts like copy
             */
            ThreadToken(ThreadToken&& token):
            _thread(token._thread)
            {}
            
            /**
             deconstructor
             */
            ~ThreadToken()
            {}
            
            /**
             checks if our Thread pointer is NULL or not
             */
            inline bl IsValid() const
            {
                return _thread != NULL;
            }
            
            /**
             invalidates our Thread pointer bu setting it to NULL
             */
            void Invalidate()
            {
                _thread = NULL;
            }
            
            /**
             gets the job associated with this token
             */
            inline Thread& GetThread()
            {
                NP_ASSERT(IsValid(), "we require a valid token when getting the thread");
                return *_thread;
            }
            
            /**
             gets the job associated with this token
             */
            inline const Thread& GetThread() const
            {
                NP_ASSERT(IsValid(), "we require a valid token when getting the thread");
                return *_thread;
            }
            
            /**
             copy assignment
             */
            ThreadToken& operator=(const ThreadToken& token)
            {
                _thread = token._thread;
                return *this;
            }
            
            /**
             move assignment - acts like copy assignment
             */
            ThreadToken& operator=(ThreadToken&& token)
            {
                _thread = token._thread;
                return *this;
            }
            
            /**
             equality operator checks if our Thread pointer equals the other Thread pointer
             */
            bl operator==(const ThreadToken& other) const
            {
                return _thread == other._thread;
            }
        };
    }
}

#endif /* NP_ENGINE_THREAD_TOKEN_HPP */
