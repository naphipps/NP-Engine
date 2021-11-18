//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/17/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_DESTROY_NATIVE_WINDOW_EVENT_HPP
#define NP_ENGINE_WINDOW_DESTROY_NATIVE_WINDOW_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"

#include "Window.hpp"

namespace np::app
{
    class WindowDestroyNativeWindowEvent : public event::Event
    {
    public:
     
        struct DataType
        {
            Window* window;
        };
        
        WindowDestroyNativeWindowEvent(Window* window)
        {
            _pad.AssignData<DataType>({window});
        }
        
        /**
         get the type that this event should be treated as
         */
        virtual event::EventType GetType() const
        {
            return event::EVENT_TYPE_WINDOW_DESTROY_NATIVE_WINDOW;
        }
        
        /**
         get the category this event should be treated as
         */
        virtual event::EventCategory GetCategory() const
        {
            return event::EVENT_CATEGORY_WINDOW;
        }
    };
}

#endif /* NP_ENGINE_WINDOW_DESTROY_NATIVE_WINDOW_EVENT_HPP */
