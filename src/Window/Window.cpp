//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Window/WindowEvents.hpp"

namespace np::win
{
	void Window::HandleClose(evnt::Event& e)
	{
		if (e.GetData<WindowCloseEvent::DataType>().window == this)
		{
			e.SetCanBeHandled();

			if (!_show_procedure_is_complete.load(mo_acquire))
			{
				if (_glfw_window != nullptr && glfwWindowShouldClose(_glfw_window) == GLFW_FALSE)
					glfwSetWindowShouldClose(_glfw_window, GLFW_TRUE);
			}
			else
			{
				_thread.Dispose();
				e.SetHandled();
			}
		}
	}

	void Window::HandleResize(evnt::Event& e)
	{
		if (e.GetData<WindowResizeEvent::DataType>().window == this)
			e.SetHandled();
	}

	void Window::ShowProcedure()
	{
		while (!glfwWindowShouldClose(_glfw_window))
		{
			thr::ThisThread::yield();
			thr::ThisThread::sleep_for(tim::Milliseconds(NP_ENGINE_WINDOW_LOOP_DURATION));
		}

		_show_procedure_is_complete.store(true, mo_release);
	}

	void Window::Close()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowCloseEvent>(*this);
	}

	void Window::Resize(ui32 width, ui32 height)
	{
		if (IsRunning())
		{
			_properties.Width = width;
			_properties.Height = height;

			glfwSetWindowSize(_glfw_window, (i32)width, (i32)height);

			_services.GetEventSubmitter().Emplace<WindowResizeEvent>(*this, width, height);
		}
	}

	void Window::SetGlfwCallbacks(GLFWwindow* glfw_window)
	{
		glfwSetWindowCloseCallback(glfw_window, WindowCloseCallback);
		glfwSetWindowSizeCallback(glfw_window, WindowSizeCallback);
		glfwSetWindowPosCallback(glfw_window, WindowPositionCallback);
	}
} // namespace np::win