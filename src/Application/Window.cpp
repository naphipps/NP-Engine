//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//


#include "NP-Engine/Application/Window.hpp"
#include "NP-Engine/Application/WindowEvents.hpp"

namespace np::app
{
    void Window::HandleSpawnNative(event::Event& event)
    {
        if (event.RetrieveData<WindowSpawnNativeWindowEvent::DataType>().window == this)
        {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            _glfw_window = glfwCreateWindow(_properties.Width, _properties.Height, _properties.Title.c_str(), nullptr, nullptr);
            event.SetHandled();
        }
    }

    void Window::HandleDestroyNative(event::Event& event)
    {
        if (event.RetrieveData<WindowDestroyNativeWindowEvent::DataType>().window == this)
        {
            glfwDestroyWindow(_glfw_window);
            _glfw_window = nullptr;
            event.SetHandled();
        }
    }

    void Window::HandleClose(event::Event& event)
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

    void Window::ShowProcedure()
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
}