//
//  EventImpl.hpp
//  Project Space
//
//  Created by Nathan Phipps on 2/9/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_EVENT_IMPL_HPP
#define NP_ENGINE_EVENT_IMPL_HPP

#include "Primitive/Primitive.hpp"
#include "Concurrency/Concurrency.hpp"

namespace np
{
    namespace event
    {
        using EventType = ui64;
        constexpr static EventType EVENT_TYPE_NONE = 0;
        constexpr static EventType EVENT_TYPE_WINDOW_CLOSE = 1;
        constexpr static EventType EVENT_TYPE_WINDOW_RESIZE = 2;
        constexpr static EventType EVENT_TYPE_WINDOW_FOCUS = 3;
        constexpr static EventType EVENT_TYPE_WINDOW_LOST_FOCUS = 4;
        constexpr static EventType EVENT_TYPE_WINDOW_MOVED = 5;
        constexpr static EventType EVENT_TYPE_KEY_PRESSED = 6;
        constexpr static EventType EVENT_TYPE_KEY_RELEASED = 7;
        constexpr static EventType EVENT_TYPE_KEY_REPREATED = 8;
        constexpr static EventType EVENT_TYPE_MOUSE_BUTTON_PRESSED = 9;
        constexpr static EventType EVENT_TYPE_MOUSE_BUTTON_RELEASED = 10;
        constexpr static EventType EVENT_TYPE_MOUSE_MOVED = 11;
        constexpr static EventType EVENT_TYPE_MOUSE_SCROLLED = 12;
        
        /**
         determines if EventType a matches b
         */
        constexpr static bl EventTypeMatches(EventType a, EventType b)
        {
            return a == b;
        }
        
        using EventCategory = ui64;
        constexpr static EventCategory EVENT_CATEGORY_NONE = 0;
        constexpr static EventCategory EVENT_CATEGORY_APPLICATION = BIT(0);
        constexpr static EventCategory EVENT_CATEGORY_WINDOW = BIT(1);
        constexpr static EventCategory EVENT_CATEGORY_INPUT = BIT(2);
        constexpr static EventCategory EVENT_CATEGORY_KEY = BIT(3);
        constexpr static EventCategory EVENT_CATEGORY_MOUSE = BIT(4);
        constexpr static EventCategory EVENT_CATEGORY_MOUSE_BUTTON = BIT(5) | EVENT_CATEGORY_MOUSE;
        
        /**
         determines if EventCategory a contains b
         */
        constexpr static bl EventCategoryContains(EventCategory a, EventCategory b)
        {
            return (a | b) != EVENT_CATEGORY_NONE;
        }
        
        /**
         represents an event
         */
        class Event
        {
        public:
            using Callback = void(*)(Event&); //TODO: figure out ehat this is for...
            
        protected:
            bl _handled;
            concurrency::PadObject _pad;
            
        public:
            
            /**
             constructor
             */
            Event():
            _handled(false)
            {}
            
            /**
             deconstructor
             */
            virtual ~Event() = default;
            
            /**
             sets whether this event has been handled or not
             */
            void SetHandled(bl handled = true)
            {
                _handled = handled;
            }
            
            /**
             gets whether this event has been handled or not
             */
            bl GetHandled() const
            {
                return _handled;
            }
            
            /**
             retrieves the value or struct from the padding of this JobFunction
             */
            template <typename T>
            const T& RetrieveData() const
            {
                return _pad.RetrieveData<T>();
            }
            
            /**
             get the type that this event should be treated as
             */
            virtual EventType GetType() const = 0;
            
            /**
             get the category this event should be treated as
             */
            virtual EventCategory GetCategory() const = 0;
        };
    }
}

#endif /* NP_ENGINE_EVENT_IMPL_HPP */
