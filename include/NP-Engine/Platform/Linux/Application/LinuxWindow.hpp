//
//  LinuxWindow.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 11/2/21.
//

#ifndef NP_ENGINE_LINUX_WINDOW_HPP
#define NP_ENGINE_LINUX_WINDOW_HPP

#include <GLFW/glfw3.h>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Application/Application.hpp"

namespace np
{
    namespace app
    {
        class LinuxWindow : public Window
        {
        private:

            GLFWwindow* _glfw_window;
            concurrency::Thread _thread;
            bl _show_procedure_is_complete;

            void HandleSpawnNative(event::Event& event)
            {
                if (event.RetrieveData<WindowSpawnNativeWindowEvent::DataType>().window == this)
                {
                    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                    _glfw_window = glfwCreateWindow(_properties.Width, _properties.Height, _properties.Title.c_str(), nullptr, nullptr);
                    event.SetHandled();
                }
            }

            void HandleDestroyNative(event::Event& event)
            {
                if (event.RetrieveData<WindowDestroyNativeWindowEvent::DataType>().window == this)
                {
                    glfwDestroyWindow(_glfw_window);
                    _glfw_window = nullptr;
                    event.SetHandled();
                }
            }

            void HandleClose(event::Event& event)
            {
                if (event.RetrieveData<WindowCloseEvent::DataType>().window == this)
                {
                    if (_glfw_window != nullptr)
                    {
                        glfwSetWindowShouldClose(_glfw_window, GLFW_TRUE);
                    }

                    if (_show_procedure_is_complete)
                    {
                        _thread.Dispose();
                        event.SetHandled();
                    }
                }
            }

            virtual void HandleEvent(event::Event& event) override
            {
                switch (event.GetType())
                {
                case event::EVENT_TYPE_WINDOW_CLOSE:
                    HandleClose(event);
                    break;
                case event::EVENT_TYPE_WINDOW_SPAWN_NATIVE_WINDOW:
                    HandleSpawnNative(event);
                    break;
                case event::EVENT_TYPE_WINDOW_DESTROY_NATIVE_WINDOW:
                    HandleDestroyNative(event);
                    break;
                }
            }

            void ShowProcedure()
            {
                _event_submitter.Emplace<WindowSpawnNativeWindowEvent>(this);
                while (_glfw_window == nullptr)
                {
                    concurrency::ThisThread::yield();
                    concurrency::ThisThread::sleep_for(time::Milliseconds(8));
                }

                while (!glfwWindowShouldClose(_glfw_window))
                {
                    concurrency::ThisThread::yield();
                    concurrency::ThisThread::sleep_for(time::Milliseconds(8));
                }

                _event_submitter.Emplace<WindowDestroyNativeWindowEvent>(this);
                while (_glfw_window != nullptr)
                {
                    concurrency::ThisThread::yield();
                    concurrency::ThisThread::sleep_for(time::Milliseconds(8));
                }

                _show_procedure_is_complete = true;
            }
            
        public:
            
            LinuxWindow(const Window::Properties& properties, event::EventSubmitter& event_submitter) :
            Window(properties, event_submitter),
            _glfw_window(nullptr),
            _show_procedure_is_complete(true)
            {}
            
            void Show() override
            {
                _show_procedure_is_complete = false;
                _thread.Run(&LinuxWindow::ShowProcedure, this);
            }

            bl IsRunning() const override
            {
                return !_show_procedure_is_complete;
            }

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

            virtual void* GetNativeWindow() const override
            {
                return _glfw_window;
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

#endif /* NP_ENGINE_LINUX_WINDOW_HPP */
