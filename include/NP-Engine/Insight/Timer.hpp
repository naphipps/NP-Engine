//
//  Timer.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/19/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_TIMER_HPP
#define NP_ENGINE_TIMER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"

namespace np
{
    namespace insight
    {
        /**
         timer class is our standard timer that collects the start time upon construction, and end time on Stop()
         */
        class Timer
        {
        protected:
            str _name;
            time::SteadyTimestamp _start_timestamp;
            time::SteadyTimestamp _end_timestamp;
            
        public:
            
            /**
             constructor
             */
            Timer(str name = ""):
            _start_timestamp(time::SteadyClock::now()),
            _name(name)
            {
                _end_timestamp = _start_timestamp;
            }
            
            /**
             deconstructor
             */
            virtual ~Timer()
            {}
            
            /**
             restarts the timer
             */
            virtual void Restart()
            {
                _start_timestamp = time::SteadyClock::now();
                _end_timestamp = _start_timestamp;
            }
            
            /**
             stops the timer
             */
            virtual void Stop()
            {
                _end_timestamp = time::SteadyClock::now();
            }
            
            /**
             gets the start time point
             */
            time::SteadyTimestamp GetStartTimestamp()
            {
                return _start_timestamp;
            }
            
            /**
             gets the end time point
             this will equal the start time point until the timer has stopped
             */
            time::SteadyTimestamp GetEndTimestamp()
            {
                return _end_timestamp;
            }
            
            /**
             sets the name of this timer
             */
            void SetName(str name)
            {
                _name = name;
            }
            
            /**
             gets the name of this timer
             */
            str GetName()
            {
                return _name;
            }
            
            /**
             get the milliseconds duration that this time has elapsed
             this will be zero until the timer has been stopped
             */
            time::DurationMilliseconds GetElapsedMilliseconds()
            {
                time::DurationMilliseconds start(_start_timestamp.time_since_epoch());
                time::DurationMilliseconds end(_end_timestamp.time_since_epoch());
                return end - start;
            }
            
            /**
             get the microseconds duration that this time has elapsed
             this will be zero until the timer has been stopped
             */
            time::DurationMicroseconds GetElapsedMicroseconds()
            {
                time::DurationMicroseconds start(_start_timestamp.time_since_epoch());
                time::DurationMicroseconds end(_end_timestamp.time_since_epoch());
                return end - start;
            }
        };
    }
}

#endif /* NP_ENGINE_TIMER_HPP */
