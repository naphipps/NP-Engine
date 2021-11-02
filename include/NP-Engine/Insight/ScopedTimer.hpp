//
//  ScopedTimer.hpp
//  Project Space
//
//  Created by Nathan Phipps on 5/19/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_SCOPED_TIMER_HPP
#define NP_ENGINE_SCOPED_TIMER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Timer.hpp"

namespace np
{
    namespace insight
    {
        /**
         ScopedTimer is our standard timer but calls Stop() upon deconstruction
         in order to collect analytics upon Stop(), this class will trigger the given scoped_timer_action
         */
        class ScopedTimer : public Timer
        {
        public:
            using Action = void(*)(ScopedTimer&);
            
        private:
            Action _stopped_action;
            bl _stopped;
            
        public:
            
            /**
             constructor
             */
            ScopedTimer(Action action = NULL):
            ScopedTimer("", action)
            {}
            
            /**
             constructor
             */
            ScopedTimer(str name, Action action = NULL):
            Timer(name),
            _stopped_action(action),
            _stopped(false)
            {}
            
            /**
             deconstructor
             */
            virtual ~ScopedTimer()
            {
                if (!_stopped)
                {
                    Stop();
                }
            }
            
            /**
             sets the given action
             this action will be called upon Stop()
             this timer will need to be restarted in order for this action to be triggered again on Stop()
             if this timer has not been stopped by deconstruction, then it will be called upon deconstruction
             */
            void SetStoppedAction(Action action)
            {
                _stopped_action = action;
            }
            
            /**
             stops this timer and calls the given scoped timer action
             this will not call the given scoped timer action again until this timer has been restarted
             */
            void Stop() override
            {
                if (!_stopped)
                {
                    Timer::Stop();
                    _stopped = true;
                    
                    if (_stopped_action)
                    {
                        _stopped_action(*this);
                    }
                }
            }
            
            /**
             restarts this timer
             */
            void Restart() override
            {
                Timer::Restart();
                _stopped = false;
            }
        };
    }
}

#endif /* NP_ENGINE_SCOPED_TIMER_HPP */
