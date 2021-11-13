//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/13/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_EVENT_HANDLER_HPP
#define NP_ENGINE_EVENT_HANDLER_HPP

#include "EventImpl.hpp"

//TODO: add summary comments

namespace np::event
{
    class EventHandler 
    {
    protected:

        virtual void HandleEvent(Event& event) = 0;

    public:

        virtual void OnEvent(Event& event)
        {
            if (EventCategoryContains(GetHandledCategories(), event.GetCategory()))
            {
                HandleEvent(event);
            }
        }

        virtual EventCategory GetHandledCategories() const = 0;
    };
}

#endif /* NP_ENGINE_EVENT_HANDLER_HPP */