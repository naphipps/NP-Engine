//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/17/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_DESTROY_NATIVE_WINDOW_EVENT_HPP
#define NP_ENGINE_WINDOW_DESTROY_NATIVE_WINDOW_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"

#include "Window.hpp"

namespace np::window
{
    class WindowDestroyNativeWindowEvent : public event::Event
    {
    public:
     
        struct DataType
        {
            Window* window;
        };
        
        WindowDestroyNativeWindowEvent(Window* window):
        event::Event()
        {
            AssignData<DataType>({window});
        }
        
        virtual event::EventType GetType() const override
        {
            return event::EVENT_TYPE_WINDOW_DESTROY_NATIVE_WINDOW;
        }
        
        virtual event::EventCategory GetCategory() const override
        {
            return event::EVENT_CATEGORY_WINDOW;
        }
    };
}

#endif /* NP_ENGINE_WINDOW_DESTROY_NATIVE_WINDOW_EVENT_HPP */
