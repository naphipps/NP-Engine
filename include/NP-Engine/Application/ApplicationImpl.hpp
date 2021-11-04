//
//  ApplicationImpl.hpp
//  Project Space
//
//  Created by Nathan Phipps on 2/9/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_APPLICATION_IMPL_HPP
#define NP_ENGINE_APPLICATION_IMPL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Renderer/Renderer.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "Layer.hpp"
#include "LayerContainer.hpp"
#include "Window.hpp"
#include "WindowEvents.hpp"

namespace np
{
    namespace app
    {
        //TODO: add summary comments
        
        class Application
        {
        public:

            struct Properties
            {
                str Title = "";
                memory::Allocator& Allocator;
            };

        protected:

            Window* _window; //TODO: uptr??
            Properties _properties;
            LayerContainer _layers;
            bl _running;
            bl _minimized;
            time::SteadyTimestamp _last_frame_timestamp;
            
            //TODO: singleton check?? --extremely rare
            //TODO: figure out how to force this into a fixed step loop - aka, set this to loop at 60fps or something with 0 being infinitely fast
            
            Application(const Application::Properties& application_properties):
            Application(application_properties, 
                Window::Properties{application_properties.Title, Window::DEFAULT_WIDTH, Window::DEFAULT_HEIGHT})
            {}

            Application(const Application::Properties& application_properties, const Window::Properties& window_properties):
            _properties(application_properties),
            _minimized(false),
            _running(false)
            {
                _window = CreateWindow(_properties.Allocator, window_properties);
                renderer::Renderer::Init();
            }
            
        public:

            virtual ~Application()
            {
                if (_window)
                {
                    memory::Destruct(_window);
                    _properties.Allocator.Deallocate(_window);
                }
                
                renderer::Renderer::Shutdown();
            }
            
        public:
            
            virtual void Run()
            {
                _running = true;
                
                while(_running)
                {
                    time::SteadyTimestamp current_timestamp = time::SteadyClock::now();
                    time::DurationMilliseconds timestamp_diff = current_timestamp - _last_frame_timestamp;
                    _last_frame_timestamp = current_timestamp;
                    
                    if (!_minimized)
                    {
                        for (ui32 i=0; i<_layers.Size(); i++)
                        {
                            _layers[i]->Update(timestamp_diff);
                        }
                        
                        /*
                         //TODO: implement imgui? I think the update above should handle imgui stuff...
                        for (ui32 i=0; i<_layers.Size(); i++)
                        {
                            _layers[i]->RenderImGui();
                        }
                         */
                    }
                    
                    //TODO: figure out how to fix our loops to 60fps
                    
                    _window->Update();
                }
            }
            
            void StopRunning()
            {
                _running = false;
            }
            
            void PushLayer(Layer* layer)
            {
                _layers.PushLayer(layer);
            }
            
            void PushOverlay(Layer* overlay)
            {
                _layers.PushOverlay(overlay);
            }
            
            void OnEvent(event::Event& e)
            {
                
            }
            
            bl OnWindowClose(WindowCloseEvent& e)
            {
                StopRunning();
                return true;
            }
            
            bl OnWindowResize(WindowResizeEvent& e)
            {
                _minimized = e.GetWidth() == 0 || e.GetHeight() == 0;
                
                if (!_minimized)
                {
                    renderer::Renderer::ResizeWindow(e.GetWidth(), e.GetHeight());
                }
                
                return !_minimized;
            }
            
            Window& GetWindow()
            {
                NP_ASSERT(_window, "_window must be defined");
                return *_window;
            }
            
            bl IsRunning() const
            {
                return _running;
            }
            
            bl IsMinimized() const
            {
                return _minimized;
            }
            
            
        };
        
        Application* CreateApplication(memory::Allocator& application_allocator);
    }
}

#endif /* NP_ENGINE_APPLICATION_IMPL_HPP */
