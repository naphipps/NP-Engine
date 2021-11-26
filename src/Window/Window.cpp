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
			if (_glfw_window != nullptr)
			{
				glfwSetWindowShouldClose(_glfw_window, GLFW_TRUE);
			}

			if (_show_procedure_is_complete.load(mo_acquire))
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
} // namespace np::window