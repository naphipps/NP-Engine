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

        virtual event::EventType GetType() const
        {
            return event::EVENT_TYPE_APPLICATION_CLOSE;
        }

        virtual event::EventCategory GetCategory() const
        {
            return event::EVENT_CATEGORY_APPLICATION;
        }
    };
}

#endif /* NP_ENGINE_APPLICATION_CLOSE_EVENT_HPP */
