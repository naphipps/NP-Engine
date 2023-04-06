//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Window/Interface/WindowInterface.hpp"

#include "NP-Engine/Window/Detail/Glfw/GlfwWindow.hpp"

namespace np::win
{
	void Window::Init(WindowDetailType detail_type)
	{
		switch (detail_type)
		{
		case WindowDetailType::Glfw:
			__detail::GlfwWindow::Init();
			break;

		default:
			break;
		}
	}

	void Window::Terminate(WindowDetailType detail_type)
	{
		switch (detail_type)
		{
		case WindowDetailType::Glfw:
			__detail::GlfwWindow::Terminate();
			break;

		default:
			break;
		}
	}

	void Window::Update(WindowDetailType detail_type)
	{
		switch (detail_type)
		{
		case WindowDetailType::Glfw:
			__detail::GlfwWindow::Update();
			break;

		default:
			break;
		}
	}

	con::vector<str> Window::GetRequiredGfxExtentions(WindowDetailType detail_type)
	{
		con::vector<str> extensions;

		switch (detail_type)
		{
		case WindowDetailType::Glfw:
			extensions = __detail::GlfwWindow::GetRequiredGfxExtentions();
			break;

		default:
			break;
		}

		return extensions;
	}

	mem::sptr<Window> Window::Create(WindowDetailType detail_type, srvc::Services& services, Window::Properties& properties)
	{
		mem::sptr<Window> window = nullptr;

		switch (detail_type)
		{
		case WindowDetailType::Glfw:
			window = mem::create_sptr<__detail::GlfwWindow>(services.GetAllocator(), properties, services);
			break;

		default:
			break;
		}

		return window;
	}

	void Window::InvokeResizeCallbacks(ui32 width, ui32 height)
	{
		if (IsRunning())
		{
			_services.GetEventSubmitter().Emplace<WindowResizeEvent>(GetUid(), width, height);

			for (auto it = _resize_callbacks.begin(); it != _resize_callbacks.end(); it++)
				(*it)(nullptr, width, height);

			for (auto it = _resize_caller_callbacks.begin(); it != _resize_caller_callbacks.end(); it++)
				it->second(it->first, width, height);
		}
	}

	void Window::InvokePositionCallbacks(i32 x, i32 y)
	{
		if (IsRunning())
		{
			_services.GetEventSubmitter().Emplace<WindowPositionEvent>(GetUid(), x, y);

			for (auto it = _position_callbacks.begin(); it != _position_callbacks.end(); it++)
				(*it)(nullptr, x, y);

			for (auto it = _position_caller_callbacks.begin(); it != _position_caller_callbacks.end(); it++)
				it->second(it->first, x, y);
		}
	}

	void Window::InvokeFramebufferCallbacks(ui32 width, ui32 height)
	{
		if (IsRunning())
		{
			_services.GetEventSubmitter().Emplace<WindowFramebufferEvent>(GetUid(), width, width);

			for (auto it = _framebuffer_callbacks.begin(); it != _framebuffer_callbacks.end(); it++)
				(*it)(nullptr, width, height);

			for (auto it = _framebuffer_caller_callbacks.begin(); it != _framebuffer_caller_callbacks.end(); it++)
				it->second(it->first, width, height);
		}
	}

	void Window::InvokeMinimizeCallbacks(bl minimized)
	{
		if (IsRunning())
		{
			_services.GetEventSubmitter().Emplace<WindowMinimizeEvent>(GetUid(), minimized);

			for (auto it = _minimize_callbacks.begin(); it != _minimize_callbacks.end(); it++)
				(*it)(nullptr, minimized);

			for (auto it = _minimize_caller_callbacks.begin(); it != _minimize_caller_callbacks.end(); it++)
				it->second(it->first, minimized);
		}
	}

	void Window::InvokeMaximizeCallbacks(bl maximized)
	{
		if (IsRunning())
		{
			_services.GetEventSubmitter().Emplace<WindowMaximizeEvent>(GetUid(), maximized);

			for (auto it = _maximize_callbacks.begin(); it != _maximize_callbacks.end(); it++)
				(*it)(nullptr, maximized);

			for (auto it = _maximize_caller_callbacks.begin(); it != _maximize_caller_callbacks.end(); it++)
				it->second(it->first, maximized);
		}
	}

	void Window::InvokeFocusCallbacks(bl focused)
	{
		if (IsRunning())
		{
			_services.GetEventSubmitter().Emplace<WindowFocusEvent>(GetUid(), focused);

			for (auto it = _focus_callbacks.begin(); it != _focus_callbacks.end(); it++)
				(*it)(nullptr, focused);

			for (auto it = _focus_caller_callbacks.begin(); it != _focus_caller_callbacks.end(); it++)
				it->second(it->first, focused);
		}
	}

	void Window::ClosingProcedure(mem::Delegate& d)
	{
		//just in case _show_procedure_is_complete is still false
		while (!_show_procedure_is_complete.load(mo_acquire))
		{
			Close();
			thr::ThisThread::yield();
		}

		_thread.Clear();

		// window layer should have constructed a mem::sptr<Window> in d.GetData
		// ownership of window is now moving from this job procedure to the closed event
		_services.GetEventSubmitter().Emplace<WindowClosedEvent>(d.GetData<mem::sptr<Window>>());
		d.DestructData<mem::sptr<Window>>();
	}

	void Window::ShowProcedure()
	{
		DetailShowProcedure();

		mem::sptr<jsys::Job> closing_job = _services.GetJobSystem().CreateJob();
		closing_job->GetDelegate().SetCallback(this, ClosingCallback);
		_services.GetEventSubmitter().Emplace<WindowClosingEvent>(GetUid(), closing_job);

		_show_procedure_is_complete.store(true, mo_release);
	}

	void Window::Resize(ui32 width, ui32 height)
	{
		if (IsRunning())
		{
			_properties.width = width;
			_properties.height = height;
			_services.GetEventSubmitter().Emplace<WindowResizeEvent>(GetUid(), width, height);
		}
	}

	void Window::Minimize()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMinimizeEvent>(GetUid(), true);
	}

	void Window::RestoreFromMinimize()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMinimizeEvent>(GetUid(), false);
	}

	void Window::Maximize()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMaximizeEvent>(GetUid(), true);
	}

	void Window::RestoreFromMaximize()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowMaximizeEvent>(GetUid(), false);
	}

	void Window::Focus()
	{
		if (IsRunning())
			_services.GetEventSubmitter().Emplace<WindowFocusEvent>(GetUid(), true);
	}
} // namespace np::win