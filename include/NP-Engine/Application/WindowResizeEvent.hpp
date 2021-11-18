//
//  WindowResizeEvent.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/10/21.
//

#ifndef NP_ENGINE_WINDOW_RESIZE_EVENT_HPP
#define NP_ENGINE_WINDOW_RESIZE_EVENT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Event/Event.hpp"

namespace np::app
{
    class WindowResizeEvent : public event::Event
    {
    public:
        struct DataType
        {
            Window* window;
            ui32 width;
            ui32 height;
        };
            
        WindowResizeEvent(Window* window, ui32 width, ui32 height):
        event::Event()
        {
            _pad.AssignData<DataType>({window, width, height});
        }

        ui32 GetWidth() const
        {
            return RetrieveData<DataType>().width;
        }

        ui32 GetHeight() const
        {
            return RetrieveData<DataType>().height;
        }

        event::EventType GetType() const override
        {
            return event::EVENT_TYPE_WINDOW_RESIZE;
        }

        event::EventCategory GetCategory() const override
        {
            return event::EVENT_CATEGORY_WINDOW;
        }
    };
}

#endif /* NP_ENGINE_WINDOW_RESIZE_EVENT_HPP */
