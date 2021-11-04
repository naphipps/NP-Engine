//
//  WindowsWindow.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 11/2/21.
//

#ifndef NP_ENGINE_WINDOWS_WINDOW_HPP
#define NP_ENGINE_WINDOWS_WINDOW_HPP

#include "NP-Engine/Application/Application.hpp"

namespace np
{
    namespace app
    {
        class WindowsWindow : public Window
        {
        public:

            /**
             construcotr
             */
            WindowsWindow(const Window::Properties& properties) :
                Window(properties)
            {}

            /**
             deconstructor
             */
            ~WindowsWindow() = default;

            /**
             update method
             */
            virtual void Update() override
            {
                //TODO: implement
            }

            /**
             sets the event callback
             */
            virtual void SetEventCallback(const event::Event::Callback& callback) override
            {
                //TODO: implement
            }

            /**
             gets the native window pointer
             */
            virtual void* GetNativeWindow() const override
            {
                //TODO: implement
                return NULL;
            }
        };
    }
}

#endif /* NP_ENGINE_WINDOWS_WINDOW_HPP */
