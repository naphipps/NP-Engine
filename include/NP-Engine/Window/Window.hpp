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

namespace np::win
{
	class Window : public evnt::EventHandler
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
		using KeyCallback = void (*)(void* caller, const nput::KeyCodeState& key_code_state);
		using MouseCallback = void (*)(void* caller, const nput::MouseCodeState& mouse_code_state);
		using MousePositionCallback = void (*)(void* caller, const nput::MousePosition& mouse_potition);
		using ControllerCallback = void (*)(void* caller, const nput::ControllerCodeState& controller_state);

	protected:
		Properties _properties;
		srvc::Services& _services;
		GLFWwindow* _glfw_window;
		thr::Thread _thread;
		atm_bl _show_procedure_is_complete;
		nput::MousePosition _mouse_position;
		con::omap<void*, ResizeCallback> _resize_callbacks;
		con::omap<void*, PositionCallback> _position_callbacks;
		con::omap<void*, KeyCallback> _key_callbacks;
		con::omap<void*, MouseCallback> _mouse_callbacks;
		con::omap<void*, MousePositionCallback> _mouse_position_callbacks;
		con::omap<void*, ControllerCallback> _controller_callbacks;

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

		static void WindowKeyCallback(GLFWwindow* glfw_window, i32 key, i32 scan, i32 action, i32 modifiers)
		{
			Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
			nput::KeyCodeState state;

			// TODO: what is scancode?!

			switch (action)
			{
			case GLFW_PRESS:
			case GLFW_REPEAT:
				state.SetActivity(nput::KeyCodeState::Activity::Active);
				state.SetActivityLevel(1);
				break;

			case GLFW_RELEASE:
			default:
				state.SetActivity(nput::KeyCodeState::Activity::Inactive);
				break;
			}

			switch (key)
			{
			case GLFW_KEY_SPACE:
				state.SetCode(nput::KeyCode::Space);
				break;

			case GLFW_KEY_APOSTROPHE:
				state.SetCode(nput::KeyCode::Apostrophe);
				break;

			case GLFW_KEY_COMMA:
				state.SetCode(nput::KeyCode::Comma);
				break;

			case GLFW_KEY_KP_SUBTRACT:
			case GLFW_KEY_MINUS:
				state.SetCode(nput::KeyCode::Minus);
				break;

			case GLFW_KEY_KP_DECIMAL:
			case GLFW_KEY_PERIOD:
				state.SetCode(nput::KeyCode::Period);
				break;

			case GLFW_KEY_KP_DIVIDE:
			case GLFW_KEY_SLASH:
				state.SetCode(nput::KeyCode::ForwardSlash);
				break;

			case GLFW_KEY_KP_0:
			case GLFW_KEY_0:
				state.SetCode(nput::KeyCode::Zero);
				break;

			case GLFW_KEY_KP_1:
			case GLFW_KEY_1:
				state.SetCode(nput::KeyCode::One);
				break;

			case GLFW_KEY_KP_2:
			case GLFW_KEY_2:
				state.SetCode(nput::KeyCode::Two);
				break;

			case GLFW_KEY_KP_3:
			case GLFW_KEY_3:
				state.SetCode(nput::KeyCode::Three);
				break;

			case GLFW_KEY_KP_4:
			case GLFW_KEY_4:
				state.SetCode(nput::KeyCode::Four);
				break;

			case GLFW_KEY_KP_5:
			case GLFW_KEY_5:
				state.SetCode(nput::KeyCode::Five);
				break;

			case GLFW_KEY_KP_6:
			case GLFW_KEY_6:
				state.SetCode(nput::KeyCode::Six);
				break;

			case GLFW_KEY_KP_7:
			case GLFW_KEY_7:
				state.SetCode(nput::KeyCode::Seven);
				break;

			case GLFW_KEY_KP_8:
			case GLFW_KEY_8:
				state.SetCode(nput::KeyCode::Eight);
				break;

			case GLFW_KEY_KP_9:
			case GLFW_KEY_9:
				state.SetCode(nput::KeyCode::Nine);
				break;

			case GLFW_KEY_SEMICOLON:
				state.SetCode(nput::KeyCode::Semicolon);
				break;

			case GLFW_KEY_KP_EQUAL:
			case GLFW_KEY_EQUAL:
				state.SetCode(nput::KeyCode::Equal);
				break;

			case GLFW_KEY_A:
				state.SetCode(nput::KeyCode::A);
				break;

			case GLFW_KEY_B:
				state.SetCode(nput::KeyCode::B);
				break;

			case GLFW_KEY_C:
				state.SetCode(nput::KeyCode::C);
				break;

			case GLFW_KEY_D:
				state.SetCode(nput::KeyCode::D);
				break;

			case GLFW_KEY_E:
				state.SetCode(nput::KeyCode::E);
				break;

			case GLFW_KEY_F:
				state.SetCode(nput::KeyCode::F);
				break;

			case GLFW_KEY_G:
				state.SetCode(nput::KeyCode::G);
				break;

			case GLFW_KEY_H:
				state.SetCode(nput::KeyCode::H);
				break;

			case GLFW_KEY_I:
				state.SetCode(nput::KeyCode::I);
				break;

			case GLFW_KEY_J:
				state.SetCode(nput::KeyCode::J);
				break;

			case GLFW_KEY_K:
				state.SetCode(nput::KeyCode::K);
				break;

			case GLFW_KEY_L:
				state.SetCode(nput::KeyCode::L);
				break;

			case GLFW_KEY_M:
				state.SetCode(nput::KeyCode::M);
				break;

			case GLFW_KEY_N:
				state.SetCode(nput::KeyCode::N);
				break;

			case GLFW_KEY_O:
				state.SetCode(nput::KeyCode::O);
				break;

			case GLFW_KEY_P:
				state.SetCode(nput::KeyCode::P);
				break;

			case GLFW_KEY_Q:
				state.SetCode(nput::KeyCode::Q);
				break;

			case GLFW_KEY_R:
				state.SetCode(nput::KeyCode::R);
				break;

			case GLFW_KEY_S:
				state.SetCode(nput::KeyCode::S);
				break;

			case GLFW_KEY_T:
				state.SetCode(nput::KeyCode::T);
				break;

			case GLFW_KEY_U:
				state.SetCode(nput::KeyCode::U);
				break;

			case GLFW_KEY_V:
				state.SetCode(nput::KeyCode::V);
				break;

			case GLFW_KEY_W:
				state.SetCode(nput::KeyCode::W);
				break;

			case GLFW_KEY_X:
				state.SetCode(nput::KeyCode::X);
				break;

			case GLFW_KEY_Y:
				state.SetCode(nput::KeyCode::Y);
				break;

			case GLFW_KEY_Z:
				state.SetCode(nput::KeyCode::Z);
				break;

			case GLFW_KEY_LEFT_BRACKET:
				state.SetCode(nput::KeyCode::LeftSquareBracket);
				break;

			case GLFW_KEY_BACKSLASH:
				state.SetCode(nput::KeyCode::Backslash);
				break;

			case GLFW_KEY_RIGHT_BRACKET:
				state.SetCode(nput::KeyCode::RightSquareBracket);
				break;

			case GLFW_KEY_GRAVE_ACCENT:
				state.SetCode(nput::KeyCode::GraveAccent);
				break;

			case GLFW_KEY_ESCAPE:
				state.SetCode(nput::KeyCode::Escape);
				break;

			case GLFW_KEY_KP_ENTER:
			case GLFW_KEY_ENTER:
				state.SetCode(nput::KeyCode::Enter);
				break;

			case GLFW_KEY_TAB:
				state.SetCode(nput::KeyCode::Tab);
				break;

			case GLFW_KEY_BACKSPACE:
				state.SetCode(nput::KeyCode::Backspace);
				break;

			case GLFW_KEY_INSERT:
				state.SetCode(nput::KeyCode::Insert);
				break;

			case GLFW_KEY_DELETE:
				state.SetCode(nput::KeyCode::Delete);
				break;

			case GLFW_KEY_RIGHT:
				state.SetCode(nput::KeyCode::ArrowRight);
				break;

			case GLFW_KEY_LEFT:
				state.SetCode(nput::KeyCode::ArrowLeft);
				break;

			case GLFW_KEY_DOWN:
				state.SetCode(nput::KeyCode::ArrowDown);
				break;

			case GLFW_KEY_UP:
				state.SetCode(nput::KeyCode::ArrowUp);
				break;

			case GLFW_KEY_PAGE_UP:
				state.SetCode(nput::KeyCode::PageUp);
				break;

			case GLFW_KEY_PAGE_DOWN:
				state.SetCode(nput::KeyCode::PageDown);
				break;

			case GLFW_KEY_HOME:
				state.SetCode(nput::KeyCode::Home);
				break;

			case GLFW_KEY_END:
				state.SetCode(nput::KeyCode::End);
				break;

			case GLFW_KEY_CAPS_LOCK:
				state.SetCode(nput::KeyCode::CapsLock);
				break;

			case GLFW_KEY_SCROLL_LOCK:
				state.SetCode(nput::KeyCode::ScrollLock);
				break;

			case GLFW_KEY_NUM_LOCK:
				state.SetCode(nput::KeyCode::NumLock);
				break;

			case GLFW_KEY_PRINT_SCREEN:
				state.SetCode(nput::KeyCode::PrintScreen);
				break;

			case GLFW_KEY_PAUSE:
				state.SetCode(nput::KeyCode::Pause);
				break;

			case GLFW_KEY_F1:
				state.SetCode(nput::KeyCode::F1);
				break;

			case GLFW_KEY_F2:
				state.SetCode(nput::KeyCode::F2);
				break;

			case GLFW_KEY_F3:
				state.SetCode(nput::KeyCode::F3);
				break;

			case GLFW_KEY_F4:
				state.SetCode(nput::KeyCode::F4);
				break;

			case GLFW_KEY_F5:
				state.SetCode(nput::KeyCode::F5);
				break;

			case GLFW_KEY_F6:
				state.SetCode(nput::KeyCode::F6);
				break;

			case GLFW_KEY_F7:
				state.SetCode(nput::KeyCode::F7);
				break;

			case GLFW_KEY_F8:
				state.SetCode(nput::KeyCode::F8);
				break;

			case GLFW_KEY_F9:
				state.SetCode(nput::KeyCode::F9);
				break;

			case GLFW_KEY_F10:
				state.SetCode(nput::KeyCode::F10);
				break;

			case GLFW_KEY_F11:
				state.SetCode(nput::KeyCode::F11);
				break;

			case GLFW_KEY_F12:
				state.SetCode(nput::KeyCode::F12);
				break;

			case GLFW_KEY_F13:
				state.SetCode(nput::KeyCode::F13);
				break;

			case GLFW_KEY_F14:
				state.SetCode(nput::KeyCode::F14);
				break;

			case GLFW_KEY_F15:
				state.SetCode(nput::KeyCode::F15);
				break;

			case GLFW_KEY_F16:
				state.SetCode(nput::KeyCode::F16);
				break;

			case GLFW_KEY_F17:
				state.SetCode(nput::KeyCode::F17);
				break;

			case GLFW_KEY_F18:
				state.SetCode(nput::KeyCode::F18);
				break;

			case GLFW_KEY_F19:
				state.SetCode(nput::KeyCode::F19);
				break;

			case GLFW_KEY_F20:
				state.SetCode(nput::KeyCode::F20);
				break;

			case GLFW_KEY_F21:
				state.SetCode(nput::KeyCode::F21);
				break;

			case GLFW_KEY_F22:
				state.SetCode(nput::KeyCode::F22);
				break;

			case GLFW_KEY_F23:
				state.SetCode(nput::KeyCode::F23);
				break;

			case GLFW_KEY_F24:
				state.SetCode(nput::KeyCode::F24);
				break;

			case GLFW_KEY_F25:
				state.SetCode(nput::KeyCode::F25);
				break;

			case GLFW_KEY_KP_MULTIPLY:
				state.SetCode(nput::KeyCode::Multiply);
				break;

			case GLFW_KEY_KP_ADD:
				state.SetCode(nput::KeyCode::Add);
				break;

			case GLFW_KEY_LEFT_SHIFT:
				state.SetCode(nput::KeyCode::LeftShift);
				break;

			case GLFW_KEY_LEFT_CONTROL:
				state.SetCode(nput::KeyCode::LeftCtrl);
				break;

			case GLFW_KEY_LEFT_ALT:
				state.SetCode(nput::KeyCode::LeftAlt);
				break;

			case GLFW_KEY_LEFT_SUPER:
				state.SetCode(nput::KeyCode::LeftSuper);
				break;

			case GLFW_KEY_RIGHT_SHIFT:
				state.SetCode(nput::KeyCode::RightShift);
				break;

			case GLFW_KEY_RIGHT_CONTROL:
				state.SetCode(nput::KeyCode::RightCtrl);
				break;

			case GLFW_KEY_RIGHT_ALT:
				state.SetCode(nput::KeyCode::RightAlt);
				break;

			case GLFW_KEY_RIGHT_SUPER:
				state.SetCode(nput::KeyCode::RightSuper);
				break;

			case GLFW_KEY_MENU:
				state.SetCode(nput::KeyCode::Menu);
				break;

			case GLFW_KEY_WORLD_1:
			case GLFW_KEY_WORLD_2:
			case GLFW_KEY_UNKNOWN:
			default:
				state.SetCode(nput::KeyCode::Unkown);
				break;
			}

			window->InvokeKeyCallbacks(state);
		}

		static void WindowMouseCallback(GLFWwindow* glfw_window, i32 button, i32 action, i32 modifiers)
		{
			Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
			nput::MouseCodeState state;

			switch (action)
			{
			case GLFW_PRESS:
			case GLFW_REPEAT:
				state.SetActivity(nput::MouseCodeState::Activity::Active);
				state.SetActivityLevel(1);
				break;

			case GLFW_RELEASE:
			default:
				state.SetActivity(nput::MouseCodeState::Activity::Inactive);
				break;
			}

			switch (button)
			{
			case GLFW_MOUSE_BUTTON_LEFT:
				state.SetCode(nput::MouseCode::LeftButton);
				break;

			case GLFW_MOUSE_BUTTON_RIGHT:
				state.SetCode(nput::MouseCode::RightButton);
				break;

			case GLFW_MOUSE_BUTTON_MIDDLE:
				state.SetCode(nput::MouseCode::MiddleButton);
				break;

			default:
				state.SetCode(nput::MouseCode::Unkown);
				break;
			}

			window->InvokeMouseCallbacks(state);
		}

		static void WindowMousePositionCallback(GLFWwindow* glfw_window, dbl x, dbl y)
		{
			Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
			nput::MousePosition& position = window->GetMousePosition();
			position.SetPosition({x, y});
			window->InvokeMousePositionCallbacks(position);
		}

		static void WindowMouseEnterCallback(GLFWwindow* glfw_window, i32 entered)
		{
			Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
			nput::MousePosition& position = window->GetMousePosition();
			position.SetIsOverSurface(entered == GLFW_TRUE);
			window->InvokeMousePositionCallbacks(position);
		}

		static void WindowControllerCallback(i32 joystick_id, i32 joystick_event)
		{
			GLFWwindow* glfw_window = (GLFWwindow*)glfwGetJoystickUserPointer(joystick_id);
			Window* window = (Window*)glfwGetWindowUserPointer(glfw_window);
			nput::ControllerCodeState state;

			switch (joystick_event)
			{
			case GLFW_CONNECTED:

				break;

			case GLFW_DISCONNECTED:
			default:

				break;
			}

			// TODO: controller support in window's callbacks is not finished

			// window->InvokeControllerCallbacks(state);
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

		void InvokeKeyCallbacks(const nput::KeyCodeState& state)
		{
			for (auto it = _key_callbacks.begin(); it != _key_callbacks.end(); it++)
				it->second(it->first, state);
		}

		void InvokeMouseCallbacks(const nput::MouseCodeState& state)
		{
			for (auto it = _mouse_callbacks.begin(); it != _mouse_callbacks.end(); it++)
				it->second(it->first, state);
		}

		void InvokeMousePositionCallbacks(const nput::MousePosition& position)
		{
			for (auto it = _mouse_position_callbacks.begin(); it != _mouse_position_callbacks.end(); it++)
				it->second(it->first, position);
		}

		void InvokeControllerCallbacks(const nput::ControllerCodeState& state)
		{
			for (auto it = _controller_callbacks.begin(); it != _controller_callbacks.end(); it++)
				it->second(it->first, state);
		}

		nput::MousePosition& GetMousePosition()
		{
			return _mouse_position;
		}

		virtual void HandleClose(evnt::Event& e);

		virtual void HandleResize(evnt::Event& e);

		virtual void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::WindowClose:
				HandleClose(e);
				break;

			case evnt::EventType::WindowResize:
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
		static const chr** GetRequiredGfxExtentions(ui32* count)
		{
			return glfwGetRequiredInstanceExtensions(count);
		}

		Window(const Window::Properties& properties, srvc::Services& services):
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

		virtual void Update(tim::DblMilliseconds time_delta) {}

		virtual void SetTitle(str title)
		{
			_properties.Title = title;
			if (IsRunning() && _glfw_window != nullptr)
				glfwSetWindowTitle(_glfw_window, _properties.Title.c_str());
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

		void SetPositionCallback(void* caller, PositionCallback callback)
		{
			_position_callbacks[caller] = callback;
		}

		void SetKeyCallback(void* caller, KeyCallback callback)
		{
			_key_callbacks[caller] = callback;
		}

		void SetMouseCallback(void* caller, MouseCallback callback)
		{
			_mouse_callbacks[caller] = callback;
		}

		void SetMousePositionCallback(void* caller, MousePositionCallback callback)
		{
			_mouse_position_callbacks[caller] = callback;
		}

		void SetControllerCallback(void* caller, ControllerCallback callback)
		{
			_controller_callbacks[caller] = callback;
		}

		void UnsetResizeCallback(void* caller)
		{
			_resize_callbacks.erase(caller);
		}

		void UnsetPositionCallback(void* caller)
		{
			_position_callbacks.erase(caller);
		}

		void UnsetKeyCallback(void* caller)
		{
			_key_callbacks.erase(caller);
		}

		void UnsetMouseCallback(void* caller)
		{
			_mouse_callbacks.erase(caller);
		}

		void UnsetMousePositionCallback(void* caller)
		{
			_mouse_position_callbacks.erase(caller);
		}

		void UnsetControllerCallback(void* caller)
		{
			_controller_callbacks.erase(caller);
		}

		void UnsetAllCallbacks(void* caller)
		{
			UnsetResizeCallback(caller);
			UnsetPositionCallback(caller);
			UnsetKeyCallback(caller);
			UnsetMouseCallback(caller);
			UnsetMousePositionCallback(caller);
			UnsetControllerCallback(caller);
		}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::Window;
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_HPP */
