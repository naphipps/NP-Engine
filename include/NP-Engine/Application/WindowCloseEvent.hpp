//
//  WindowCloseEvent.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/10/21.
//

#ifndef NP_ENGINE_WINDOW_CLOSE_EVENT_HPP
#define NP_ENGINE_WINDOW_CLOSE_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"

namespace np
{
    namespace app
    {
        class WindowCloseEvent : public event::Event
        {
        public:
            
            /**
             constructor
             */
            WindowCloseEvent():
            event::Event()
            {}
            
            /**
             deconsructor
             */
            ~WindowCloseEvent() = default;
            
            /**
             gets the type of this event
             */
            event::EventType GetType() const override
            {
                return event::EVENT_TYPE_WINDOW_CLOSE;
            }
            
            /**
             gets the category of this event
             */
            event::EventCategory GetCategory() const override
            {
                return event::EVENT_CATEGORY_WINDOW;
            }
        };
    }
}


#endif /* NP_ENGINE_WINDOW_CLOSE_EVENT_HPP */
