//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/16/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_LAYER_HPP
#define NP_ENGINE_WINDOW_LAYER_HPP

#include <GLFW/glfw3.h>

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "Layer.hpp"
#include "ApplicationCloseEvent.hpp"

namespace np::app
{
	class WindowLayer : public Layer
	{
    private:

        memory::TraitAllocator _allocator;
        container::vector<window::Window*> _windows; //TODO: make this vector of pointers again...
        container::vector<void*> _native_windows;

    protected:

        virtual void HandleWindowCreate(event::Event& event)
        {
            CreateWindow(event.RetrieveData<window::Window::Properties>());
            event.SetHandled();
        }

        virtual void HandleEvent(event::Event& event) override
        {
            switch (event.GetType())
            {
            case event::EVENT_TYPE_WINDOW_CREATE:
                HandleWindowCreate(event);
                break;
            }

            for (auto it = _windows.begin(); !event.IsHandled() && it != _windows.end(); it++)
            {
                (*it)->OnEvent(event);
            }
        }

    public:

        WindowLayer(event::EventSubmitter& event_submitter):
        Layer(event_submitter)
        {
            glfwInit();
        }

        virtual ~WindowLayer()
        {
            for (auto it = _windows.begin(); it != _windows.end(); it++)
            {
                memory::Destruct(*it);
                _allocator.Deallocate(*it);
            }

            glfwTerminate();
        }

        virtual window::Window* CreateWindow(window::Window::Properties& properties);
        
        virtual void Update(time::DurationMilliseconds time_delta) override
        {
            glfwPollEvents(); //TODO: how do we move this to the main thread
            
            for (window::Window* window : _windows)
            {
                window->Update(time_delta);
            }
        }

        virtual void Cleanup() override
        {
            for (i32 i = _windows.size() - 1; i >= 0; i--)
            {
                if (!_windows[i]->IsRunning())
                {
                    _windows.erase(_windows.begin() + i);
                }
            }

//#if !NP_ENGINE_PLATFORM_IS_APPLE //TODO: ?
            if (_windows.size() == 0)
            {
                _event_submitter.Emplace<ApplicationCloseEvent>();
            }
//#endif
        }

        virtual event::EventCategory GetHandledCategories() const
        {
            return event::EVENT_CATEGORY_WINDOW;
        }
	};
}

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
