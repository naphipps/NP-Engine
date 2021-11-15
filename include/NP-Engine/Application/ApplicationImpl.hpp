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
#include "NP-Engine/Graphics/Graphics.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Layer.hpp"
#include "LayerContainer.hpp"
#include "Window.hpp"
#include "WindowEvents.hpp"

namespace np
{
    namespace app
    {
        //TODO: add summary comments
        
        class Application : public event::EventHandler
        {
        public:

            struct Properties
            {
                str Title = "";
                memory::Allocator& Allocator;
            };

        protected:

            Properties _properties;
            event::EventManager _event_manager;
            container::vector<Window*> _windows;
            LayerContainer _layers;
            bl _running;
            
            Application(const Application::Properties& application_properties):
            _properties(application_properties),
            _running(false)
            {
                _event_manager.RegisterHandler(*this);
                graphics::Init();
            }
            
            virtual void Cleanup()
            {
                if (!_windows.empty())
                {
                    for (i32 i = _windows.size() - 1; i >= 0; i--)
                    {
                        if (!_windows[i]->IsRunning())
                        {
                            Window* w = _windows[i];
                            _windows.erase(_windows.begin() + i);
                            memory::Destruct(w);
                            _windows.get_allocator().Deallocate(w);
                        }
                    }
                }

                if (_windows.empty())
                {
                    StopRunning();
                }
            }

            virtual void CreateWindow()
            {
                _windows.emplace_back(Window::Create(_windows.get_allocator(), Window::Properties{ _event_manager }));
                _windows.back()->Show();
            }

        public:

            virtual ~Application()
            {
                for (Window* window : _windows)
                {
                    memory::Destruct(window);
                    _properties.Allocator.Deallocate(window);
                }
                
                graphics::Shutdown();
            }
            
            void Run()
            {
                Run(0, nullptr);
            }

            virtual void Run(i32 argc, chr** argv)
            {
                _running = true;
                time::SteadyTimestamp _last_frame_timestamp;

                while(_running)
                {
                    time::SteadyTimestamp current_timestamp = time::SteadyClock::now();
                    time::DurationMilliseconds timestamp_diff = current_timestamp - _last_frame_timestamp;
                    _last_frame_timestamp = current_timestamp;
                    
                    _event_manager.DispatchEvents(); //TODO: is this right spot??

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
                    
                    //TODO: figure out how to fix our loops to 60fps
                    //TODO: figure out how to force this into a fixed step loop - aka, set this to loop at 60fps or something with 0 being infinitely fast
                    
                    for (Window* window : _windows)
                    {
                        window->Update(timestamp_diff);
                    }

                    Cleanup();
                }
            }

            Properties GetProperties() const
            {
                return _properties;
            }

            str GetTitle() const
            {
                return _properties.Title;
            }

            memory::Allocator& GetAllocator() const
            {
                return _properties.Allocator;
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
            
            void HandleEvent(event::Event& e) override
            {
                //TODO: close application? to close all the windows, then stop running
            }

            event::EventCategory GetHandledCategories() const override
            {
                return event::EVENT_CATEGORY_APPLICATION | event::EVENT_CATEGORY_WINDOW;
            }
            
            bl IsRunning() const
            {
                return _running;
            }
        };
        
        Application* CreateApplication(memory::Allocator& application_allocator); //TODO: make this a static function of Application?
    }
}

#endif /* NP_ENGINE_APPLICATION_IMPL_HPP */
