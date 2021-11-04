//
//  Window.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/10/21.
//

#ifndef NP_ENGINE_WINDOW_HPP
#define NP_ENGINE_WINDOW_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"

namespace np
{
    namespace app
    {
        class Window
        {
        public:

            constexpr static ui32 DEFAULT_WIDTH = 1600;
            constexpr static ui32 DEFAULT_HEIGHT = 900;

            struct Properties
            {
                str Title = "NP Window";
                ui32 Width = DEFAULT_WIDTH;
                ui32 Height = DEFAULT_HEIGHT;
            };
            
        private:
            Properties _properties;
            
        public:
            
            /**
             constructor
             */
            Window(const Window::Properties& properties):
            _properties(properties)
            {}
            
            /**
             deconstructor
             */
            virtual ~Window() = default;
            
            /**
             gets the width
             */
            ui32 GetWidth() const
            {
                return _properties.Width;
            }
            
            /**
             gets the height
             */
            ui32 GetHeight() const
            {
                return _properties.Height;
            }
            
            /**
             gets the title
             */
            str GetTitle() const
            {
                return _properties.Title;
            }
            
            /**
             gets the properties
             */
            Properties GetProperties() const
            {
                return _properties;
            }
            
            /**
             update method
             */
            virtual void Update() = 0;
            
            /**
             sets the event callback
             */
            virtual void SetEventCallback(const event::Event::Callback& callback) = 0;
            
            /**
             enables or disables vsync
             */
            virtual void SetEnableVSync(bl enabled = true)
            {}
            
            /**
             checks is this window is currently vsync-ing or not
             */
            virtual bl IsVSync() const
            {
                return false;
            }
            
            /**
             gets the native window pointer
             */
            virtual void* GetNativeWindow() const = 0;
        };
        
        Window* CreateWindow(memory::Allocator& allocator, const Window::Properties& properties = Window::Properties());
    }
}

#endif /* NP_ENGINE_WINDOW_HPP */
