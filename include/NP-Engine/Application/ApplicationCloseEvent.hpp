//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_APPLICATION_CLOSE_EVENT_HPP
#define NP_ENGINE_APPLICATION_CLOSE_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"

namespace np::app
{
    class ApplicationCloseEvent : public event::Event
    {
    public:

        ApplicationCloseEvent():
        event::Event()
        {}

        virtual event::EventType GetType() const override
        {
            return event::EVENT_TYPE_APPLICATION_CLOSE;
        }

        virtual event::EventCategory GetCategory() const override
        {
            return event::EVENT_CATEGORY_APPLICATION;
        }
    };
}

#endif /* NP_ENGINE_APPLICATION_CLOSE_EVENT_HPP */
