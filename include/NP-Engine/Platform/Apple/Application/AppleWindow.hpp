//
//  AppleWindow.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/10/21.
//

#ifndef NP_ENGINE_APPLE_WINDOW_HPP
#define NP_ENGINE_APPLE_WINDOW_HPP

#include <GLFW/glfw3.h>

#include <iostream> //TODO: remove
#include <cstdlib> //TODO: remove

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"
#include "NP-Engine/Application/Application.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/System/System.hpp"
#include "NP-Engine/Insight/Insight.hpp"

namespace np
{
    namespace app
    {
        class AppleWindow : public Window
        {
        private:
            
            static atm_ui32 _window_count;
            static Mutex _mutex;

            GLFWwindow* _glfw_window;
            concurrency::Thread _thread;
            bl _show_procedure_is_complete;
            bl _keep_showing;

            void HandleSpawnNative(event::Event& event)
            {
                if (event.RetrieveData<WindowSpawnNativeWindowEvent::DataType>().window == this)
                {
                    //Lock lock(_mutex);
                    _window_count++;

                    if (_window_count == 1)
                    {
                        
                    }

                    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                    _glfw_window = glfwCreateWindow(_properties.Width, _properties.Height, _properties.Title.c_str(), nullptr, nullptr);
                    
                    event.SetHandled();
                }
            }

            void HandleDestroyNative(event::Event& event)
            {
                if (event.RetrieveData<WindowDestroyNativeWindowEvent::DataType>().window == this)
                {
                    //Lock lock(_mutex);
                    _window_count--;

                    glfwDestroyWindow(_glfw_window);
                    
                    if (_window_count == 0)
                    {
                        
                    }
                    
                    _glfw_window = nullptr;
                    event.SetHandled();
                }
            }
            
            void HandleClose(event::Event& event)
            {
                if (event.RetrieveData<WindowCloseEvent::DataType>().window == this)
                {
                    //TODO: could we set the event as handled then send this logic into a thread? or like how can we asyncronsly handle this event?
                    _keep_showing = false;
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
                //system::Popup::Show("Hey", "we got here just fine");
//                ::std::cout<<"got here just fine\n";
                //std::abort();
                
                _event_submitter.Emplace<WindowSpawnNativeWindowEvent>(this);
                while(_glfw_window == nullptr);
                
                bl should_close = false;
                
                _keep_showing = true;
                while (_keep_showing)
                {
                    if (glfwWindowShouldClose(_glfw_window))
                    {
                        if (!should_close)
                        {
                            should_close = true;
                            _event_submitter.Emplace<WindowCloseEvent>(this);
                        }
                    }
                }
                
                _event_submitter.Emplace<WindowDestroyNativeWindowEvent>(this);
                while(_glfw_window != nullptr);
                
                _show_procedure_is_complete = true;
            }
            
        public:
            
            AppleWindow(const Window::Properties& properties, event::EventSubmitter& event_submitter):
            Window(properties, event_submitter),
            _glfw_window(nullptr),
            _show_procedure_is_complete(true),
            _keep_showing(false)
            {}
            
            virtual void Show() override
            {
                _show_procedure_is_complete = false;
                _thread.Run(&AppleWindow::ShowProcedure, this);
            }
            
            virtual bl IsRunning() const override
            {
                return _thread.IsRunning();
            }

            virtual void SetTitle(str title) override
            {
                _properties.Title = title;
                if (IsRunning() && _glfw_window != nullptr)
                {
                    glfwSetWindowTitle(_glfw_window, _properties.Title.c_str());
                }
            }
            
            virtual void Update(time::DurationMilliseconds duration_milliseconds) override
            {
                //TODO: implement
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

#endif /* NP_ENGINE_APPLE_WINDOW_HPP */
