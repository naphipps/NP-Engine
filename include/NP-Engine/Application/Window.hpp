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
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/String/String.hpp"

//TODo: update comments

namespace np
{
    namespace app
    {
        class Window : public event::EventHandler
        {
        public:

            constexpr static ui32 DEFAULT_WIDTH = 800;
            constexpr static ui32 DEFAULT_HEIGHT = 600;

            struct Properties
            {
                str Title = "NP Window";
                ui32 Width = DEFAULT_WIDTH;
                ui32 Height = DEFAULT_HEIGHT;
            };
            
        protected:
            Properties _properties;

        public:
            
            static Window* Create(memory::Allocator& allocator, const Window::Properties& properties, event::EventSubmitter& event_submitter);

            /**
             constructor
             */
            Window(const Window::Properties& properties, event::EventSubmitter& event_submitter):
            event::EventHandler(event_submitter),
            _properties(properties)
            {}

            virtual ~Window()
            {}
            
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
            virtual void Update(time::DurationMilliseconds duration_milliseconds) = 0; //TODO: should this also have a delta time param??
            
            virtual void Show() = 0;
            
            virtual bl IsRunning() const = 0;

            virtual void SetTitle(str title) = 0;

            //virtual void AttachToRenderer() = 0; //TODO: we need to attach/detach from our renderer

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

            virtual bl IsMinimized() const = 0; //TODO: or can we keep track of this with a flag??
        };
    }
}

#endif /* NP_ENGINE_WINDOW_HPP */
