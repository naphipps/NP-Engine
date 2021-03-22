//
//  WindowResizeEvent.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/10/21.
//

#ifndef NP_ENGINE_WINDOW_RESIZE_EVENT_HPP
#define NP_ENGINE_WINDOW_RESIZE_EVENT_HPP

#include "../Primitive/Primitive.hpp"
#include "../Event/Event.hpp"

namespace np
{
    namespace app
    {
        class WindowResizeEvent : public event::Event
        {
        public:
            struct DataType
            {
                ui32 Width;
                ui32 Height;
            };
            
        public:
            
            /**
             constructor
             */
            WindowResizeEvent(ui32 width, ui32 height):
            event::Event()
            {
                DataType data
                {
                    .Width = width,
                    .Height = height
                };
                _pad.AssignData(data);
            }
            
            /**
             deconstructor
             */
            ~WindowResizeEvent() = default;
            
            /**
             gets the width of the resize
             */
            ui32 GetWidth() const
            {
                return RetrieveData<DataType>().Width;
            }
            
            /**
             gets the height of the resize
             */
            ui32 GetHeight() const
            {
                return RetrieveData<DataType>().Height;
            }
            
            /**
             gets the type of this event
             */
            event::EventType GetType() const override
            {
                return event::EVENT_TYPE_WINDOW_RESIZE;
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

#endif /* NP_ENGINE_WINDOW_RESIZE_EVENT_HPP */
