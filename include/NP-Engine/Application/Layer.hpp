//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_LAYER_HPP
#define NP_ENGINE_LAYER_HPP

#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Event/Event.hpp"

namespace np::app
{
    /*
        Layer will represent a layer in our application
    */
    class Layer : public event::EventHandler
    {
    protected:

        virtual void HandleEvent(event::Event& event) {}

    public:

        Layer(event::EventSubmitter& event_submitter):
        event::EventHandler(event_submitter)
        {}

        virtual void Attach() {}

        virtual void Detach() {}

        virtual void Update(time::DurationMilliseconds time_delta) {}

        virtual void Cleanup() {}

        virtual event::EventCategory GetHandledCategories() const
        {
            return event::EVENT_CATEGORY_NONE;
        }
    };
}

#endif /* NP_ENGINE_LAYER_HPP */