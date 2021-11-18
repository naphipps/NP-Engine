//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/16/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_LAYER_HPP
#define NP_ENGINE_WINDOW_LAYER_HPP

#include <GLFW/glfw3.h>

#include "NP-Engine/Container/Container.hpp"

#include "Layer.hpp"
#include "Window.hpp"
#include "ApplicationCloseEvent.hpp"

namespace np::app
{
	class WindowLayer : public Layer
	{
    private:

        container::vector<Window*> _windows;
        container::vector<void*> _native_windows;

    protected:

        virtual void HandleEvent(event::Event& event) override
        {
            for (auto it = _windows.begin(); !event.IsHandled() && it != _windows.end(); it++)
            {
                (*it)->OnEvent(event);
            }
        }

    public:

        WindowLayer(event::EventSubmitter& event_submitter):
        WindowLayer(memory::DefaultTraitAllocator, event_submitter)
        {}

        WindowLayer(memory::Allocator& allocator, event::EventSubmitter& event_submitter):
        Layer(event_submitter),
        _windows(allocator)
        {
            glfwInit();
        }

        virtual ~WindowLayer()
        {
            for (auto it = _windows.begin(); it != _windows.end(); it++)
            {
                memory::Destruct(*it);
                _windows.get_allocator().Deallocate(*it);
            }
            
            glfwTerminate();
        }

        Window* CreateWindow()
        {
            memory::Block block = _windows.get_allocator().Allocate(sizeof(Window));
            memory::Construct<Window>(block, Window::Properties(), _event_submitter);
            return _windows.emplace_back((Window*)block.ptr);
        }
        
        virtual void Update(time::DurationMilliseconds time_delta) override
        {
            glfwPollEvents(); //TODO: how do we move this to the main thread
            
            for (Window* window : _windows)
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
                    Window* window = _windows[i];
                    _windows.erase(_windows.begin() + i);
                    memory::Destruct(window);
                    _windows.get_allocator().Deallocate(window);
                }
            }

            if (_windows.size() == 0)
            {
                _event_submitter.Emplace<ApplicationCloseEvent>();
            }
        }

        virtual event::EventCategory GetHandledCategories() const
        {
            return event::EVENT_CATEGORY_WINDOW;
        }
	};
}

#endif /* NP_ENGINE_WINDOW_LAYER_HPP */
