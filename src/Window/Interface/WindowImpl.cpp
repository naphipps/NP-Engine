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

	mem::sptr<Window> Window::Create(WindowDetailType detail_type, mem::sptr<srvc::Services> services, Window::Properties properties)
	{
		mem::sptr<Window> window = nullptr;

		switch (detail_type)
		{
		case WindowDetailType::Glfw:
			window = mem::create_sptr<__detail::GlfwWindow>(services->GetAllocator(), properties, services);
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
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowResizeEvent>(_services->GetAllocator(), GetUid(), width, height);
			_services->GetEventSubmitter().Submit(e);

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
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowPositionEvent>(_services->GetAllocator(), GetUid(), x, y);
			_services->GetEventSubmitter().Submit(e);

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
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowFramebufferEvent>(_services->GetAllocator(), GetUid(), width, width);
			_services->GetEventSubmitter().Submit(e);

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
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowMinimizeEvent>(_services->GetAllocator(), GetUid(), minimized);
			_services->GetEventSubmitter().Submit(e);

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
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowMaximizeEvent>(_services->GetAllocator(), GetUid(), maximized);
			_services->GetEventSubmitter().Submit(e);

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
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowFocusEvent>(_services->GetAllocator(), GetUid(), focused);
			_services->GetEventSubmitter().Submit(e);

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
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowClosedEvent>(_services->GetAllocator(), d.GetData<mem::sptr<Window>>());
		_services->GetEventSubmitter().Submit(e);
		d.DestructData<mem::sptr<Window>>();
	}

	void Window::ShowProcedure()
	{
		DetailShowProcedure();

		mem::sptr<jsys::Job> closing_job = _services->GetJobSystem().CreateJob();
		closing_job->GetDelegate().ConstructData<mem::sptr<Window>>();
		closing_job->GetDelegate().SetCallback(this, ClosingCallback);
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowClosingEvent>(_services->GetAllocator(), GetUid(), closing_job);
		_services->GetEventSubmitter().Submit(e);

		_show_procedure_is_complete.store(true, mo_release);
	}

	void Window::Resize(ui32 width, ui32 height)
	{
		if (IsRunning())
		{
			_properties.width = width;
			_properties.height = height;
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowResizeEvent>(_services->GetAllocator(), GetUid(), width, height);
			_services->GetEventSubmitter().Submit(e);
		}
	}

	void Window::Minimize()
	{
		if (IsRunning())
		{
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowMinimizeEvent>(_services->GetAllocator(), GetUid(), true);
			_services->GetEventSubmitter().Submit(e);
		}
	}

	void Window::RestoreFromMinimize()
	{
		if (IsRunning())
		{
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowMinimizeEvent>(_services->GetAllocator(), GetUid(), false);
			_services->GetEventSubmitter().Submit(e);
		}
	}

	void Window::Maximize()
	{
		if (IsRunning())
		{
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowMaximizeEvent>(_services->GetAllocator(), GetUid(), true);
			_services->GetEventSubmitter().Submit(e);
		}
	}

	void Window::RestoreFromMaximize()
	{
		if (IsRunning())
		{
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowMaximizeEvent>(_services->GetAllocator(), GetUid(), false);
			_services->GetEventSubmitter().Submit(e);
		}
	}

	void Window::Focus()
	{
		if (IsRunning())
		{
			mem::sptr<evnt::Event> e = mem::create_sptr<WindowFocusEvent>(_services->GetAllocator(), GetUid(), true);
			_services->GetEventSubmitter().Submit(e);
		}
	}
} // namespace np::win