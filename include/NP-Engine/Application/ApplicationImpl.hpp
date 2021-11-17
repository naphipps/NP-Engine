//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_APPLICATION_IMPL_HPP
#define NP_ENGINE_APPLICATION_IMPL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Graphics/Graphics.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "WindowLayer.hpp"
#include "Layer.hpp"
#include "Window.hpp"
#include "WindowEvents.hpp"

namespace np::app
{
    //TODO: add summary comments
        
    class Application : public Layer
    {
    public:

        struct Properties
        {
            str Title = "";
            memory::Allocator& Allocator;
        };

    protected:

        Properties _properties;
        event::EventQueue _event_queue;
        event::EventSubmitter _application_event_submitter;
        WindowLayer _window_layer;
        container::vector<Layer*> _layers;
        container::vector<Layer*> _overlays;
        atm_bl _running;
            
        Application(const Application::Properties& application_properties):
        _application_event_submitter(_event_queue), //order matters
        Layer(_application_event_submitter),
        _properties(application_properties),
        _window_layer(_properties.Allocator, _event_submitter),
        _layers(_properties.Allocator),
        _overlays(_properties.Allocator),
        _running(false)
        {
            _layers.emplace_back(this);
            _layers.emplace_back(memory::AddressOf(_window_layer));

            graphics::Init();
        }
            
        virtual WindowLayer& GetWindowLayer()
        {
            return _window_layer;
        }

        void HandleEvent(event::Event& e) override
        {
            switch (e.GetType())
            {
            case event::EVENT_TYPE_APPLICATION_CLOSE:
                std::cout << "handled application close event\n";
                StopRunning();
                e.SetHandled();
                break;
            }
        }

    public:

        virtual ~Application()
        {
            graphics::Shutdown();
        }
            
        void Run()
        {
            Run(0, nullptr);
        }

        virtual void Run(i32 argc, chr** argv)
        {
            _running.store(true, mo_release);
            time::SteadyTimestamp _last_frame_timestamp;

            while(_running.load(mo_acquire))
            {
                time::SteadyTimestamp current_timestamp = time::SteadyClock::now();
                time::DurationMilliseconds timestamp_diff = current_timestamp - _last_frame_timestamp;
                _last_frame_timestamp = current_timestamp;
                    
                for (Layer* layer : _layers)
                {
                    layer->Update(timestamp_diff);
                }

                for (Layer* overlay : _overlays)
                {
                    overlay->Update(timestamp_diff);
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
                    
                for (event::Event* e = _event_queue.PopOther(); e != nullptr; e = _event_queue.PopOther())
                {
                    for (auto it = _overlays.rbegin(); !e->IsHandled() && it != _overlays.rend(); it++)
                    {
                        (*it)->OnEvent(*e);
                    }

                    for (auto it = _layers.rbegin(); !e->IsHandled() && it != _layers.rend(); it++)
                    {
                        (*it)->OnEvent(*e);
                    }

                    if (e->IsHandled())
                    {
                        _event_queue.DestroyEvent(e);
                    }
                    else
                    {
                        _event_queue.Emplace(e);
                    }
                }

                _event_queue.SwapBuffers();

                for (Layer* layer : _layers)
                {
                    layer->Cleanup();
                }

                for (Layer* overlay : _overlays)
                {
                    overlay->Cleanup();
                }
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
            _running.store(false, mo_release);
        }
            
        void PushLayer(Layer* layer) //TODO: protected??
        {
            _layers.emplace_back(layer);
        }
            
        void PushOverlay(Layer* overlay) //TODO: protected??
        {
            _overlays.emplace_back(overlay);
        }
            
        event::EventCategory GetHandledCategories() const override
        {
            return event::EVENT_CATEGORY_APPLICATION;
        }
            
        bl IsRunning() const
        {
            return _running.load(mo_acquire);
        }
    };
        
    Application* CreateApplication(memory::Allocator& application_allocator); //TODO: make this a static function of Application?
}

#endif /* NP_ENGINE_APPLICATION_IMPL_HPP */
