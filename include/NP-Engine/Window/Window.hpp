//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_HPP
#define NP_ENGINE_WINDOW_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/GlfwInclude.hpp"

namespace np::window
{
	class Window : public event::EventHandler
	{
	public:
		constexpr static ui32 DEFAULT_WIDTH = 800;
		constexpr static ui32 DEFAULT_HEIGHT = 600;

		struct Properties
		{
			str Title = "NP Window";
			ui32 Width = DEFAULT_WIDTH;
			ui32 Height = DEFAULT_HEIGHT;
		};

		using ResizeCallback = void (*)(void* caller, ui32 width, ui32 height);
		using PositionCallback = void (*)(void* caller, i32 x, i32 y);

	protected:
		Properties _properties;
		services::Services& _services;
		GLFWwindow* _glfw_window;
		thr::Thread _thread;
		atm_bl _show_procedure_is_complete;
		container::omap<void*, ResizeCallback> _resize_callbacks;
		container::omap<void*, PositionCallback> _position_callbacks;

		static void WindowCloseCallback(GLFWwindow* glfw_window)
		{
			((Window*)glfwGetWindowUserPointer(glfw_window))->Close();
		}

		static void WindowSizeCallback(GLFWwindow* glfw_window, i32 width, i32 height)
		{
			Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
			window->InvokeResizeCallbacks(width, height);
		}

		static void WindowPositionCallback(GLFWwindow* glfw_window, i32 x, i32 y)
		{
			Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
			window->InvokePositionCallbacks(x, y);
		}

		void InvokeResizeCallbacks(ui32 width, ui32 height)
		{
			for (auto it = _resize_callbacks.begin(); it != _resize_callbacks.end(); it++)
				it->second(it->first, width, height);
		}

		void InvokePositionCallbacks(i32 x, i32 y)
		{
			for (auto it = _position_callbacks.begin(); it != _position_callbacks.end(); it++)
				it->second(it->first, x, y);
		}

		virtual void HandleClose(event::Event& e);

		virtual void HandleResize(event::Event& e);

		virtual void HandleEvent(event::Event& e) override
		{
			switch (e.GetType())
			{
			case event::EventType::WindowClose:
				HandleClose(e);
				break;
			case event::EventType::WindowResize:
				HandleResize(e);
				break;
			default:
				break;
			}
		}

		virtual void ShowProcedure();

		void SetGlfwCallbacks(GLFWwindow* glfw_window);

		GLFWwindow* CreateGlfwWindow()
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			GLFWwindow* glfw_window =
				glfwCreateWindow(_properties.Width, _properties.Height, _properties.Title.c_str(), nullptr, nullptr);
			glfwSetWindowUserPointer(glfw_window, this);
			SetGlfwCallbacks(glfw_window);
			return glfw_window;
		}

	public:
		Window(const Window::Properties& properties, services::Services& services):
			_properties(properties),
			_services(services),
			_glfw_window(CreateGlfwWindow()),
			_show_procedure_is_complete(true)
		{
			Show();
		}

		virtual ~Window()
		{
			if (_glfw_window != nullptr)
				glfwDestroyWindow(_glfw_window);
		}

		virtual void Show()
		{
			_show_procedure_is_complete.store(false, mo_release);
			_thread.Run(&Window::ShowProcedure, this);
		}

		virtual void Close();

		virtual void Resize(ui32 width, ui32 height);

		virtual bl IsRunning() const
		{
			return !_show_procedure_is_complete.load(mo_acquire) || _thread.IsRunning();
		}

		virtual ui32 GetWidth() const
		{
			return _properties.Width;
		}

		virtual ui32 GetHeight() const
		{
			return _properties.Height;
		}

		virtual str GetTitle() const
		{
			return _properties.Title;
		}

		virtual Properties GetProperties() const
		{
			return _properties;
		}

		virtual void Update(time::DurationMilliseconds duration_milliseconds) {}

		virtual void SetTitle(str title)
		{
			_properties.Title = title;
			if (IsRunning() && _glfw_window != nullptr)
			{
				glfwSetWindowTitle(_glfw_window, _properties.Title.c_str());
			}
		}

		virtual void SetEnableVSync(bl enabled = true) {}

		virtual void* GetNativeWindow() const
		{
			return _glfw_window;
		}

		void SetResizeCallback(void* caller, ResizeCallback callback)
		{
			_resize_callbacks[caller] = callback;
		}

		void UnsetResizeCallback(void* caller)
		{
			_resize_callbacks.erase(caller);
		}

		void SetPositionCallback(void* caller, PositionCallback callback)
		{
			_position_callbacks[caller] = callback;
		}

		void UnsetPositionCallback(void* caller)
		{
			_position_callbacks.erase(caller);
		}

		void UnsetAllCallbacks(void* caller)
		{
			UnsetResizeCallback(caller);
			UnsetPositionCallback(caller);
		}

		virtual event::EventCategory GetHandledCategories() const override
		{
			return event::EventCategory::Window;
		}
	};
} // namespace np::window

#endif /* NP_ENGINE_WINDOW_HPP */
