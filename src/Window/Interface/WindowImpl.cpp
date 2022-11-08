//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Window/Interface/WindowInterface.hpp"

#include "NP-Engine/Window/Detail/Glfw/GlfwWindow.hpp"

namespace np::win
{
	void Window::Init()
	{
		switch (__detail::RegisteredWindowDetailType.load(mo_acquire))
		{
		case WindowDetailType::Glfw:
			__detail::GlfwWindow::Init();
			break;

		default:
			break;
		}
	}

	void Window::Terminate()
	{
		switch (__detail::RegisteredWindowDetailType.load(mo_acquire))
		{
		case WindowDetailType::Glfw:
			__detail::GlfwWindow::Terminate();
			break;

		default:
			break;
		}
	}

	void Window::Update()
	{
		switch (__detail::RegisteredWindowDetailType.load(mo_acquire))
		{
		case WindowDetailType::Glfw:
			__detail::GlfwWindow::Update();
			break;

		default:
			break;
		}
	}

	con::vector<str> Window::GetRequiredGfxExtentions()
	{
		con::vector<str> extensions;

		switch (__detail::RegisteredWindowDetailType.load(mo_acquire))
		{
		case WindowDetailType::Glfw:
			extensions = __detail::GlfwWindow::GetRequiredGfxExtentions();
			break;

		default:
			break;
		}

		return extensions;
	}

	Window* Window::Create(srvc::Services& services, const Window::Properties& properties)
	{
		Window* window = nullptr;

		switch (__detail::RegisteredWindowDetailType.load(mo_acquire))
		{
		case WindowDetailType::Glfw:
			window = mem::Create<__detail::GlfwWindow>(services.GetAllocator(), properties, services);
			break;

		default:
			break;
		}

		return window;
	}

	void Window::InvokeResizeCallbacks(ui32 width, ui32 height)
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowResizeEvent>(this, width, height);

		for (auto it = _resize_callbacks.begin(); it != _resize_callbacks.end(); it++)
			it->second(it->first, width, height);
	}

	void Window::InvokePositionCallbacks(i32 x, i32 y)
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowPositionEvent>(this, x, y);

		for (auto it = _position_callbacks.begin(); it != _position_callbacks.end(); it++)
			it->second(it->first, x, y);
	}

	void Window::InvokeFramebufferCallbacks(ui32 width, ui32 height)
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowFramebufferEvent>(this, width, width);

		for (auto it = _framebuffer_callbacks.begin(); it != _framebuffer_callbacks.end(); it++)
			it->second(it->first, width, height);
	}

	void Window::InvokeMinimizeCallbacks(bl minimized)
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMinimizeEvent>(this, minimized);

		for (auto it = _minimize_callbacks.begin(); it != _minimize_callbacks.end(); it++)
			it->second(it->first, minimized);
	}

	void Window::InvokeMaximizeCallbacks(bl maximized)
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMaximizeEvent>(this, maximized);

		for (auto it = _maximize_callbacks.begin(); it != _maximize_callbacks.end(); it++)
			it->second(it->first, maximized);
	}

	void Window::InvokeFocusCallbacks(bl focused)
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowFocusEvent>(this, focused);

		for (auto it = _focus_callbacks.begin(); it != _focus_callbacks.end(); it++)
			it->second(it->first, focused);
	}

	void Window::ClosingProcedure(mem::Delegate& d)
	{
		while (!_show_procedure_is_complete.load(mo_acquire))
		{
			Close();
			thr::ThisThread::yield();
		}

		_thread.Dispose();

		//ownership of window is now moving from this job procedure to the closed event
		_services.GetEventSubmitter().Emplace<WindowClosedEvent>(this);
	}

	void Window::ShowProcedure()
	{
		DetailShowProcedure();
		
		jsys::Job* window_closing_job = _services.GetJobSystem().CreateJob();
		window_closing_job->GetDelegate().Connect<Window, &Window::ClosingProcedure >(this);
		_services.GetEventSubmitter().Emplace<WindowClosingEvent>(this, window_closing_job);

		_show_procedure_is_complete.store(true, mo_release);
	}

	void Window::Resize(ui32 width, ui32 height)
	{
		if (IsRunning())
		{
			_properties.Width = width;
			_properties.Height = height;
			_services.GetEventSubmitter().Emplace<WindowResizeEvent>(this, width, height);
		}
	}

	void Window::Minimize()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMinimizeEvent>(this, true);
	}

	void Window::RestoreFromMinimize()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMinimizeEvent>(this, false);
	}

	void Window::Maximize()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMaximizeEvent>(this, true);
	}

	void Window::RestoreFromMaximize()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMaximizeEvent>(this, false);
	}

	void Window::Focus()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowFocusEvent>(this, true);
	}
} // namespace np::win