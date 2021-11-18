//
//  WindowsWindow.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 11/2/21.
//

#ifndef NP_ENGINE_WINDOWS_WINDOW_HPP
#define NP_ENGINE_WINDOWS_WINDOW_HPP

#include <GLFW/glfw3.h>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"

#include "NP-Engine/Application/Application.hpp"

namespace np
{
    namespace app
    {
        class WindowsWindow : public Window
        {
        private:

            static ui32 _window_count;
            static Mutex _mutex;

            GLFWwindow* _glfw_window;
            concurrency::Thread _thread;
            bl _show_procedure_is_complete;
            bl _keep_showing;

            void HandleClose(event::Event& event)
            {
                if (event.RetrieveData<WindowCloseEvent::DataType>().window == this)
                {
                    //TODO: could we set the event as handled then send this logic into a thread? or like how can we asyncronsly handle this event?
                    _keep_showing = false;
                    while (!_show_procedure_is_complete);
                    _thread.Dispose();
                    event.SetHandled();
                }
            }

            virtual void HandleEvent(event::Event& event) override
            {
                switch (event.GetType())
                {
                case event::EVENT_TYPE_WINDOW_CLOSE:
                    HandleClose(event);
                    break;
                }
            }

            void CreateGlfwWindow()
            {
                Lock lock(_mutex);
                _window_count++;

                if (_window_count == 1)
                {
                    glfwInit();
                }

                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                _glfw_window = glfwCreateWindow(_properties.Width, _properties.Height, _properties.Title.c_str(), nullptr, nullptr);
            }

            void DestroyGlfwWindow()
            {
                Lock lock(_mutex);
                _window_count--;

                glfwDestroyWindow(_glfw_window);

                if (_window_count == 0)
                {
                    glfwTerminate();
                }
            }

            void ShowProcedure()
            {
                CreateGlfwWindow();
                _keep_showing = true;
                while (_keep_showing)
                {
                    if (glfwWindowShouldClose(_glfw_window))
                    {
                        _event_submitter.Emplace<WindowCloseEvent>(this);
                    }

                    glfwPollEvents();
                }
                DestroyGlfwWindow();
                _show_procedure_is_complete = true;
            }

        public:

            /**
             construcotr
             */
            WindowsWindow(const Window::Properties& properties, event::EventSubmitter& event_submitter) :
            Window(properties, event_submitter),
            _glfw_window(nullptr),
            _show_procedure_is_complete(true),
            _keep_showing(false)
            {}
                        
            void Show() override
            {
                _show_procedure_is_complete = false;
                _thread.Run(&WindowsWindow::ShowProcedure, this);
            }

            bl IsRunning() const override
            {
                return _thread.IsRunning();
            }

            /**
             update method
             */
            virtual void Update(time::DurationMilliseconds duratrion_milliseconds) override
            {
                //TODO: implement
            }

            virtual void SetTitle(str title) override
            {
                _properties.Title = title;
                if (IsRunning() && _glfw_window != nullptr)
                {
                    glfwSetWindowTitle(_glfw_window, _properties.Title.c_str());
                }
            }

            /**
             gets the native window pointer
             */
            virtual void* GetNativeWindow() const override
            {
                return nullptr; //TODO: implement
            }

            virtual event::EventCategory GetHandledCategories() const override
            {
                return event::EVENT_CATEGORY_WINDOW;
            }

            virtual bl IsMinimized() const override
            {
                return false; //TODO: figure this out
            }
        };
    }
}

#endif /* NP_ENGINE_WINDOWS_WINDOW_HPP */
