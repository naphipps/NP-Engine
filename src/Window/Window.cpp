//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Window/Window.hpp"
#include "NP-Engine/Window/WindowEvents.hpp"

namespace np::window
{
	void Window::HandleClose(event::Event& event)
	{
		if (event.RetrieveData<WindowCloseEvent::DataType>().window == this)
		{
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
				event.SetHandled();
			}
		}
	}

	void Window::ShowProcedure()
	{
		while (!glfwWindowShouldClose(_glfw_window))
		{
			concurrency::ThisThread::yield();
			concurrency::ThisThread::sleep_for(time::Milliseconds(8));
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

	void Window::SetGlfwCallbacks(GLFWwindow* glfw_window)
	{
		glfwSetWindowCloseCallback(glfw_window,
			[](GLFWwindow* w)
			{
				((Window*)glfwGetWindowUserPointer(w))->Close();
			}
		);
	}
} // namespace np::window