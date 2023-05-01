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

	mem::sptr<Window> Window::Create(WindowDetailType detail_type, mem::sptr<srvc::Services> services)
	{
		mem::sptr<Window> window = nullptr;

		switch (detail_type)
		{
		case WindowDetailType::Glfw:
			window = mem::create_sptr<__detail::GlfwWindow>(services->GetAllocator(), services);
			break;

		default:
			break;
		}

		return window;
	}

	void Window::InvokeResizeCallbacks(ui32 width, ui32 height)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowSizeEvent>(_services->GetAllocator(), GetUid(), width, height);
		_services->GetEventSubmitter().Submit(e);

		{
			SizeCallbacksAccess callbacks = _size_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, width, height);
		}
		{
			SizeCallerCallbacksAccess callbacks = _size_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, width, height);
		}
	}

	void Window::InvokePositionCallbacks(i32 x, i32 y)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowPositionEvent>(_services->GetAllocator(), GetUid(), x, y);
		_services->GetEventSubmitter().Submit(e);

		{
			PositionCallbacksAccess callbacks = _position_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, x, y);
		}
		{
			PositionCallerCallbacksAccess callbacks = _position_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, x, y);
		}
	}

	void Window::InvokeFramebufferCallbacks(ui32 width, ui32 height)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowFramebufferEvent>(_services->GetAllocator(), GetUid(), width, width);
		_services->GetEventSubmitter().Submit(e);

		{
			FramebufferCallbacksAccess callbacks = _framebuffer_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, width, height);
		}
		{
			FramebufferCallerCallbacksAccess callbacks = _framebuffer_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, width, height);
		}
	}

	void Window::InvokeMinimizeCallbacks(bl minimized)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowMinimizeEvent>(_services->GetAllocator(), GetUid(), minimized);
		_services->GetEventSubmitter().Submit(e);

		{
			MinimizeCallbacksAccess callbacks = _minimize_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, minimized);
		}
		{
			MinimizeCallerCallbacksAccess callbacks = _minimize_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, minimized);
		}
	}

	void Window::InvokeMaximizeCallbacks(bl maximized)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowMaximizeEvent>(_services->GetAllocator(), GetUid(), maximized);
		_services->GetEventSubmitter().Submit(e);

		{
			MaximizeCallbacksAccess callbacks = _maximize_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, maximized);
		}
		{
			MaximizeCallerCallbacksAccess callbacks = _maximize_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, maximized);
		}
	}

	void Window::InvokeFocusCallbacks(bl focused)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowFocusEvent>(_services->GetAllocator(), GetUid(), focused);
		_services->GetEventSubmitter().Submit(e);

		{
			FocusCallbacksAccess callbacks = _focus_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, focused);
		}
		{
			FocusCallerCallbacksAccess callbacks = _focus_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, focused);
		}
	}
} // namespace np::win