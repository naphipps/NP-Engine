//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Window/WindowEvents.hpp"

namespace np::window
{
	void Window::HandleClose(event::Event& e)
	{
		if (e.RetrieveData<WindowCloseEvent::DataType>().window == this)
		{
			e.SetCanBeHandled();

			if (!_show_procedure_is_complete.load(mo_acquire))
			{
				if (_glfw_window != nullptr && glfwWindowShouldClose(_glfw_window) == GLFW_FALSE)
				{
					glfwSetWindowShouldClose(_glfw_window, GLFW_TRUE);
				}
			}
			else
			{
				_thread.Dispose();
				e.SetHandled();
			}
		}
	}

	void Window::HandleResize(event::Event& e)
	{
		if (e.RetrieveData<WindowResizeEvent::DataType>().window == this)
		{
			e.SetHandled();
		}
	}

	void Window::ShowProcedure()
	{
		while (!glfwWindowShouldClose(_glfw_window))
		{
			concurrency::ThisThread::yield();
			concurrency::ThisThread::sleep_for(time::Milliseconds(NP_ENGINE_WINDOW_LOOP_DURATION));
		}

		_show_procedure_is_complete.store(true, mo_release);
	}

	void Window::Close()
	{
		if (IsRunning())
		{
			_event_submitter.Emplace<WindowCloseEvent>(*this);
		}
	}

	void Window::Resize(ui32 width, ui32 height)
	{
		if (IsRunning())
		{
			_properties.Width = width;
			_properties.Height = height;

			glfwSetWindowSize(_glfw_window, (i32)width, (i32)height);

			_event_submitter.Emplace<WindowResizeEvent>(*this, width, height);
		}
	}

	void Window::SetGlfwCallbacks(GLFWwindow* glfw_window)
	{
		glfwSetWindowCloseCallback(glfw_window, WindowCloseCallback);
		glfwSetWindowSizeCallback(glfw_window, WindowSizeCallback);
		glfwSetWindowPosCallback(glfw_window, WindowPositionCallback);
	}
} // namespace np::window