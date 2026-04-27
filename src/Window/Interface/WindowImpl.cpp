//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Window/Interface/Interface.hpp"

#include "NP-Engine/Window/Detail/Glfw/GlfwWindow.hpp"

namespace np::win
{
	void Window::Init(DetailType detail_type)
	{
		switch (detail_type)
		{
		case DetailType::Glfw:
			__detail::GlfwWindow::Init();
			break;

		default:
			break;
		}
	}

	void Window::Terminate(DetailType detail_type)
	{
		switch (detail_type)
		{
		case DetailType::Glfw:
			__detail::GlfwWindow::Terminate();
			break;

		default:
			break;
		}
	}

	void Window::Update(DetailType detail_type)
	{
		switch (detail_type)
		{
		case DetailType::Glfw:
			__detail::GlfwWindow::Update();
			break;

		default:
			break;
		}
	}

	con::vector<str> Window::GetRequiredGpuExtentions(DetailType detail_type)
	{
		con::vector<str> extensions;

		switch (detail_type)
		{
		case DetailType::Glfw:
			extensions = __detail::GlfwWindow::GetRequiredGpuExtentions();
			break;

		default:
			break;
		}

		return extensions;
	}

	mem::sptr<Window> Window::Create(DetailType detail_type, mem::sptr<srvc::Services> services, uid::Uid id)
	{
		mem::sptr<Window> window = nullptr;

		switch (detail_type)
		{
		case DetailType::Glfw:
			window = mem::create_sptr<__detail::GlfwWindow>(services->GetAllocator(), services, id);
			break;

		default:
			break;
		}

		return window;
	}

	void Window::InvokeSizeCallbacks(::glm::uvec2 size)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowSizeEvent>(_services->GetAllocator(), evnt::EventType::Did, GetUid(), size);
		_services->GetEventSubmitter().Submit(e);

		{
			auto callbacks = _size_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, size);
		}
		{
			auto callbacks = _size_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, size);
		}
	}

	void Window::InvokePositionCallbacks(::glm::ivec2 position)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowPositionEvent>(_services->GetAllocator(), evnt::EventType::Did, GetUid(), position);
		_services->GetEventSubmitter().Submit(e);

		{
			auto callbacks = _position_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, position);
		}
		{
			auto callbacks = _position_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, position);
		}
	}

	void Window::InvokeFramebufferSizeCallbacks(::glm::uvec2 framebuffer_size)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowFramebufferSizeEvent>(_services->GetAllocator(), evnt::EventType::Did, GetUid(), framebuffer_size);
		_services->GetEventSubmitter().Submit(e);

		{
			auto callbacks = _framebuffer_size_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, framebuffer_size);
		}
		{
			auto callbacks = _framebuffer_size_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, framebuffer_size);
		}
	}

	void Window::InvokeMinimizeCallbacks(bl minimized)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowMinimizeEvent>(_services->GetAllocator(), evnt::EventType::Did, GetUid(), minimized);
		_services->GetEventSubmitter().Submit(e);

		{
			auto callbacks = _minimize_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, minimized);
		}
		{
			auto callbacks = _minimize_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, minimized);
		}
	}

	void Window::InvokeMaximizeCallbacks(bl maximized)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowMaximizeEvent>(_services->GetAllocator(), evnt::EventType::Did, GetUid(), maximized);
		_services->GetEventSubmitter().Submit(e);

		{
			auto callbacks = _maximize_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, maximized);
		}
		{
			auto callbacks = _maximize_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, maximized);
		}
	}

	void Window::InvokeFocusCallbacks(bl focused)
	{
		mem::sptr<evnt::Event> e = mem::create_sptr<WindowFocusEvent>(_services->GetAllocator(), evnt::EventType::Did, GetUid(), focused);
		_services->GetEventSubmitter().Submit(e);

		{
			auto callbacks = _focus_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				(*it)(nullptr, focused);
		}
		{
			auto callbacks = _focus_caller_callbacks.get_access();
			for (auto it = callbacks->begin(); it != callbacks->end(); it++)
				it->second(it->first, focused);
		}
	}
} // namespace np::win