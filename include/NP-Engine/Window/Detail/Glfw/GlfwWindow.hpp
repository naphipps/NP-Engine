//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GLFW_WINDOW_HPP
#define NP_ENGINE_GLFW_WINDOW_HPP

#if NP_ENGINE_PLATFORM_IS_WINDOWS
	#include <dwmapi.h>
#endif

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "NP-Engine/Vendor/GlfwInclude.hpp"
#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "NP-Engine/Window/Interface/WindowInterface.hpp"

// TODO: resizing window smaller than min siz results with mouse offset to be incorrect
// TODO: something about dragging the window and then quickly closing it causes race condition
//	^ between dragging logic and closing logic

namespace np::win::__detail
{
	class GlfwWindow : public Window
	{
	protected:
		atm<GLFWwindow*> _glfw_window;
		nput::MousePosition _mouse_position;
		tim::SteadyTimestamp _apply_system_theme_timestamp;

#if NP_ENGINE_PLATFORM_IS_WINDOWS

		using HitFlags = ui32;
		constexpr static HitFlags HitNone = 0;
		constexpr static HitFlags HitCaption = BIT(0);
		constexpr static HitFlags HitTop = BIT(1);
		constexpr static HitFlags HitRight = BIT(2);
		constexpr static HitFlags HitBottom = BIT(3);
		constexpr static HitFlags HitLeft = BIT(4);
		constexpr static HitFlags HitMinimize = BIT(5);
		constexpr static HitFlags HitMaximize = BIT(6);
		constexpr static HitFlags HitClose = BIT(7);

		WNDPROC _prev_window_procedure;
		HitFlags _hit_flags;
		POINT _prev_mouse_position;
		WINDOWPLACEMENT _prev_window_placement;

		static LRESULT CALLBACK GlfwWindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
		{
			LRESULT result = -1; // assuming -1 is an error value
			GlfwWindow* glfw_window = (GlfwWindow*)GetWindowLongPtrA(hwnd, GWLP_USERDATA);
			bl call_prev_window_procedure = true;
			POINTS point = MAKEPOINTS(lparam); // screen point

			switch (msg)
			{
			case WM_NCLBUTTONDBLCLK:
				GetWindowPlacement(hwnd, &glfw_window->_prev_window_placement);
			case WM_NCRBUTTONDBLCLK:
				call_prev_window_procedure = GET_NCHITTEST_WPARAM(wparam) != HTSYSMENU;
				break;

			case WM_NCRBUTTONDOWN:
				call_prev_window_procedure = false;
				break;

			case WM_NCLBUTTONDOWN:
			{
				switch (GET_NCHITTEST_WPARAM(wparam))
				{
				case HTCAPTION:
					glfw_window->_hit_flags = HitCaption;
					break;

				case HTTOP:
					glfw_window->_hit_flags = HitTop;
					break;

				case HTLEFT:
					glfw_window->_hit_flags = HitLeft;
					break;

				case HTRIGHT:
					glfw_window->_hit_flags = HitRight;
					break;

				case HTBOTTOM:
					glfw_window->_hit_flags = HitBottom;
					break;

				case HTTOPLEFT:
					glfw_window->_hit_flags = HitTop | HitLeft;
					break;

				case HTTOPRIGHT:
					glfw_window->_hit_flags = HitTop | HitRight;
					break;

				case HTBOTTOMLEFT:
					glfw_window->_hit_flags = HitLeft | HitBottom;
					break;

				case HTBOTTOMRIGHT:
					glfw_window->_hit_flags = HitRight | HitBottom;
					break;

				case HTMINBUTTON:
					glfw_window->_hit_flags = HitMinimize;
					break;

				case HTMAXBUTTON:
					glfw_window->_hit_flags = HitMaximize;
					break;

				case HTCLOSE:
					glfw_window->_hit_flags = HitClose;
					break;

				case HTSYSMENU:
				{
					call_prev_window_procedure = false;
					break;
				}
				default:
					glfw_window->_hit_flags = HitNone;
					break;
				}

				if (glfw_window->_hit_flags)
				{
					glfw_window->_prev_mouse_position.x = point.x;
					glfw_window->_prev_mouse_position.y = point.y;
					call_prev_window_procedure = false;
					result = 0; // assuming 0 is a success value
				}

				break;
			}
			case WM_NCLBUTTONUP:
			{
				switch (GET_NCHITTEST_WPARAM(wparam))
				{
				case HTMINBUTTON:
				{
					if (glfw_window->_hit_flags == HitMinimize)
					{
						glfw_window->Minimize();
						call_prev_window_procedure = false;
					}
					break;
				}
				case HTMAXBUTTON:
				{
					if (glfw_window->_hit_flags == HitMaximize)
					{
						if (glfw_window->IsMaximized())
							glfw_window->RestoreFromMaximize();
						else
							glfw_window->Maximize();
						call_prev_window_procedure = false;
					}
					break;
				}
				case HTCLOSE:
				{
					if (glfw_window->_hit_flags == HitClose)
					{
						glfw_window->Close();
						call_prev_window_procedure = false;
					}
					break;
				}
				default:
					break;
				}

				glfw_window->_hit_flags = HitNone;
				break;
			}
			}

			if (call_prev_window_procedure)
				result = CallWindowProcA(glfw_window->_prev_window_procedure, hwnd, msg, wparam, lparam);

			return result;
		}
#endif

		static void WindowCloseCallback(GLFWwindow* glfw_window)
		{
			((GlfwWindow*)glfwGetWindowUserPointer(glfw_window))->Close();
		}

		static void WindowSizeCallback(GLFWwindow* glfw_window, i32 width, i32 height)
		{
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			window->InvokeResizeCallbacks(width, height);
		}

		static void WindowPositionCallback(GLFWwindow* glfw_window, i32 x, i32 y)
		{
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			window->InvokePositionCallbacks(x, y);
		}

		static void FramebufferSizeCallback(GLFWwindow* glfw_window, i32 width, i32 height)
		{
			if (width < 0)
				width = 0;

			if (height < 0)
				height = 0;

			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			window->InvokeResizeCallbacks(width, height);
		}

		static void WindowIconifyCallback(GLFWwindow* glfw_window, i32 iconified)
		{
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			window->InvokeMinimizeCallbacks(iconified);
		}

		static void WindowMaximizeCallback(GLFWwindow* glfw_window, i32 maximized)
		{
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			window->InvokeMaximizeCallbacks(maximized);
		}

		static void WindowFocusCallback(GLFWwindow* glfw_window, i32 focused)
		{
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			window->InvokeFocusCallbacks(focused);
		}

		static void KeyCallback(GLFWwindow* glfw_window, i32 key, i32 scan, i32 action, i32 modifiers)
		{
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			nput::KeyCodeState state;

			// TODO: what is scancode?!

			switch (action)
			{
			case GLFW_PRESS:
			case GLFW_REPEAT:
				state.SetIsActive(true);
				state.SetActivityLevel(1);
				break;

			case GLFW_RELEASE:
			default:
				state.SetIsActive(false);
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

		static void MouseButtonCallback(GLFWwindow* glfw_window, i32 button, i32 action, i32 modifiers)
		{
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			nput::MouseCodeState state;

			switch (action)
			{
			case GLFW_PRESS:
			case GLFW_REPEAT:
				state.SetIsActive(true);
				state.SetActivityLevel(1);
				break;

			case GLFW_RELEASE:
			default:
				state.SetIsActive(false);
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

		static void MousePositionCallback(GLFWwindow* glfw_window, dbl x, dbl y)
		{
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			window->_mouse_position.SetPosition({x, y});
			window->InvokeMousePositionCallbacks(window->_mouse_position);
		}

		static void MouseEnterCallback(GLFWwindow* glfw_window, i32 entered)
		{
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
			window->_mouse_position.SetIsOverSurface(entered);
			window->InvokeMousePositionCallbacks(window->_mouse_position);
		}

		static void ControllerCallback(i32 joystick_id, i32 joystick_event)
		{
			GLFWwindow* glfw_window = (GLFWwindow*)glfwGetJoystickUserPointer(joystick_id);
			GlfwWindow* window = (GlfwWindow*)glfwGetWindowUserPointer(glfw_window);
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

		void SetGlfwCallbacks(GLFWwindow* glfw_window)
		{
			if (glfw_window)
			{
				glfwSetWindowUserPointer(glfw_window, this);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_1, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_2, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_3, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_4, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_5, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_6, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_7, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_8, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_9, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_10, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_11, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_12, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_13, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_14, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_15, glfw_window);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_16, glfw_window);

				glfwSetWindowCloseCallback(glfw_window, WindowCloseCallback);
				glfwSetWindowSizeCallback(glfw_window, WindowSizeCallback);
				glfwSetWindowPosCallback(glfw_window, WindowPositionCallback);
				glfwSetFramebufferSizeCallback(glfw_window, FramebufferSizeCallback);
				glfwSetWindowIconifyCallback(glfw_window, WindowIconifyCallback);
				glfwSetWindowMaximizeCallback(glfw_window, WindowMaximizeCallback);
				glfwSetWindowFocusCallback(glfw_window, WindowFocusCallback);
				glfwSetKeyCallback(glfw_window, KeyCallback);
				glfwSetMouseButtonCallback(glfw_window, MouseButtonCallback);
				glfwSetCursorPosCallback(glfw_window, MousePositionCallback);
				glfwSetCursorEnterCallback(glfw_window, MouseEnterCallback);
				glfwSetJoystickCallback(ControllerCallback);

#if NP_ENGINE_PLATFORM_IS_WINDOWS
				HWND native_window = (HWND)GetNativeFromGlfw(glfw_window);
				LONG_PTR prev_window_procedure = SetWindowLongPtrA(native_window, GWLP_WNDPROC, (LONG_PTR)&GlfwWindowProcedure);
				SetWindowLongPtrA(native_window, GWLP_USERDATA, (LONG_PTR)this);
				_prev_window_procedure = (WNDPROC)prev_window_procedure;
#endif
			}
		}

		void UnsetGlfwCallbacks(GLFWwindow* glfw_window)
		{
			if (glfw_window)
			{
				glfwSetWindowUserPointer(glfw_window, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_1, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_2, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_3, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_4, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_5, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_6, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_7, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_8, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_9, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_10, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_11, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_12, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_13, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_14, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_15, nullptr);
				glfwSetJoystickUserPointer(GLFW_JOYSTICK_16, nullptr);

				glfwSetWindowCloseCallback(glfw_window, nullptr);
				glfwSetWindowSizeCallback(glfw_window, nullptr);
				glfwSetWindowPosCallback(glfw_window, nullptr);
				glfwSetFramebufferSizeCallback(glfw_window, nullptr);
				glfwSetWindowIconifyCallback(glfw_window, nullptr);
				glfwSetWindowMaximizeCallback(glfw_window, nullptr);
				glfwSetWindowFocusCallback(glfw_window, nullptr);
				glfwSetKeyCallback(glfw_window, nullptr);
				glfwSetMouseButtonCallback(glfw_window, nullptr);
				glfwSetCursorPosCallback(glfw_window, nullptr);
				glfwSetCursorEnterCallback(glfw_window, nullptr);
				glfwSetJoystickCallback(nullptr);

#if NP_ENGINE_PLATFORM_IS_WINDOWS
				HWND native_window = (HWND)GetNativeFromGlfw(glfw_window);
				if (native_window)
				{
					SetWindowLongPtrA(native_window, GWLP_WNDPROC, (LONG_PTR)_prev_window_procedure);
					SetWindowLongPtrA(native_window, GWLP_USERDATA, (LONG_PTR) nullptr);
				}
				_prev_window_procedure = nullptr;
#endif
			}
		}

		void DetailShowProcedure() override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			const tim::Ui64Milliseconds duration(NP_ENGINE_WINDOW_LOOP_DURATION);
			while (!glfwWindowShouldClose(glfw_window))
				thr::ThisThread::sleep_for(duration);
		}

		void DetailCloseProcedure() override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (glfw_window && glfwWindowShouldClose(glfw_window) == GLFW_FALSE)
				glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
		}

		void ClosingProcedure(mem::Delegate& d) override
		{
			Window::ClosingProcedure(d);

			GLFWwindow* glfw_window = nullptr;
			if (_glfw_window.compare_exchange_strong(glfw_window, nullptr))
				DestroyGlfwWindow(glfw_window);
		}

		GLFWwindow* CreateGlfwWindow()
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			GLFWwindow* glfw_window = glfwCreateWindow(GetWidth(), GetHeight(), GetTitle().c_str(), nullptr, nullptr);
			SetGlfwCallbacks(glfw_window);
			ApplySystemThemeOnGlfw(glfw_window);
			return glfw_window;
		}

		void ApplySystemThemeOnGlfw(GLFWwindow* glfw_window) // TODO: maybe add "void ApplySystemThemeOnGlfw()" to Window??
		{
			if (glfw_window)
			{
#if NP_ENGINE_PLATFORM_IS_WINDOWS
				LPCSTR subkey = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize";
				LPCSTR value = "SystemUsesLightTheme";
				DWORD word = 0;
				DWORD size = sizeof(DWORD);
				LSTATUS status = RegGetValueA(HKEY_CURRENT_USER, subkey, value, RRF_RT_DWORD, nullptr, &word, &size);
				HWND native_window = (HWND)GetNativeFromGlfw(glfw_window);
				BOOL dark_mode_value = status == ERROR_SUCCESS && word == 0;
				BOOL prev_dark_mode_value = false;
				DWORD DARK_MODE_ATTRIBUTE = 20;

				DwmGetWindowAttribute(native_window, DARK_MODE_ATTRIBUTE, &prev_dark_mode_value, sizeof(BOOL));

				if (prev_dark_mode_value != dark_mode_value)
				{
					DwmSetWindowAttribute(native_window, DARK_MODE_ATTRIBUTE, &dark_mode_value, sizeof(BOOL));

					// TODO: test this
					bl is_active = native_window == GetActiveWindow();
					GlfwWindowProcedure(native_window, WM_NCACTIVATE, !is_active, 0);
					GlfwWindowProcedure(native_window, WM_NCACTIVATE, is_active, 0);
				}
#endif
			}
		}

		void DestroyGlfwWindow(GLFWwindow* glfw_window)
		{
			if (glfw_window)
			{
				UnsetGlfwCallbacks(glfw_window);
				glfwDestroyWindow(glfw_window);
			}
		}

		void* GetNativeFromGlfw(GLFWwindow* glfw_window) const
		{
			return glfw_window ? glfwGetWin32Window(glfw_window) : nullptr;
		}

	public:
		static void Init()
		{
			glfwInit();
		}

		static void Terminate()
		{
			glfwTerminate();
		}

		static void Update()
		{
			glfwPollEvents();
		}

		static con::vector<str> GetRequiredGfxExtentions()
		{
			con::vector<str> extensions;
			ui32 count = 0;
			const chr** exts = glfwGetRequiredInstanceExtensions(&count);

			for (ui32 i = 0; i < count; i++)
				extensions.emplace_back(exts[i]);

			return extensions;
		}

		GlfwWindow(Window::Properties& properties, srvc::Services& services):
			Window(properties, services),
			_glfw_window(nullptr),
			_apply_system_theme_timestamp(tim::SteadyClock::now())
#if NP_ENGINE_PLATFORM_IS_WINDOWS
			,
			_prev_window_procedure(nullptr),
			_hit_flags(HitNone)
#endif
		{
			Show();
		}

		virtual ~GlfwWindow()
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			while (!_glfw_window.compare_exchange_weak(glfw_window, nullptr))
				DestroyGlfwWindow(glfw_window);
		}

		virtual void Update(tim::DblMilliseconds milliseconds) override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (glfw_window)
			{
#if NP_ENGINE_PLATFORM_IS_WINDOWS
				HWND native_window = (HWND)GetNativeWindow();
				POINT point;
				WINDOWINFO info{};
				UINT flags = 0;
				bl call_set_window_pos = false;
				GetCursorPos(&point);
				GetWindowInfo(native_window, &info);

				::glm::i32vec2 offset{point.x - _prev_mouse_position.x, point.y - _prev_mouse_position.y};

				switch (_hit_flags)
				{
				case HitCaption:
					if (IsMaximized())
					{
						if (offset.x != 0 || offset.y != 0)
						{
							dbl t = mat::GetT((dbl)info.rcWindow.left, (dbl)info.rcWindow.right, (dbl)point.x);
							siz width =
								_prev_window_placement.rcNormalPosition.right - _prev_window_placement.rcNormalPosition.left;
							siz height =
								_prev_window_placement.rcNormalPosition.bottom - _prev_window_placement.rcNormalPosition.top;

							_prev_window_placement.rcNormalPosition.left = point.x - (t * width) + offset.x;
							_prev_window_placement.rcNormalPosition.top = offset.y;
							_prev_window_placement.rcNormalPosition.right =
								_prev_window_placement.rcNormalPosition.left + width;
							_prev_window_placement.rcNormalPosition.bottom =
								_prev_window_placement.rcNormalPosition.top + height;

							_prev_window_placement.showCmd |= SW_RESTORE;
							SetWindowPlacement(native_window, &_prev_window_placement);
						}
						// else do nothing on purpose
					}
					else
					{
						info.rcWindow.left += offset.x;
						info.rcWindow.top += offset.y;
						flags = SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW;
						call_set_window_pos = true;
					}
					break;

				case HitTop:
					info.rcWindow.top += offset.y;
					flags = SWP_NOZORDER | SWP_SHOWWINDOW;
					call_set_window_pos = true;
					break;

				case HitBottom:
					info.rcWindow.bottom += offset.y;
					flags = SWP_NOZORDER | SWP_SHOWWINDOW;
					call_set_window_pos = true;
					break;

				case HitLeft:
					info.rcWindow.left += offset.x;
					flags = SWP_NOZORDER | SWP_SHOWWINDOW;
					call_set_window_pos = true;
					break;

				case HitRight:
					info.rcWindow.right += offset.x;
					flags = SWP_NOZORDER | SWP_SHOWWINDOW;
					call_set_window_pos = true;
					break;

				case HitTop | HitLeft:
					info.rcWindow.top += offset.y;
					info.rcWindow.left += offset.x;
					flags = SWP_NOZORDER | SWP_SHOWWINDOW;
					call_set_window_pos = true;
					break;

				case HitTop | HitRight:
					info.rcWindow.top += offset.y;
					info.rcWindow.right += offset.x;
					flags = SWP_NOZORDER | SWP_SHOWWINDOW;
					call_set_window_pos = true;
					break;

				case HitBottom | HitLeft:
					info.rcWindow.bottom += offset.y;
					info.rcWindow.left += offset.x;
					flags = SWP_NOZORDER | SWP_SHOWWINDOW;
					call_set_window_pos = true;
					break;

				case HitBottom | HitRight:
					info.rcWindow.bottom += offset.y;
					info.rcWindow.right += offset.x;
					flags = SWP_NOZORDER | SWP_SHOWWINDOW;
					call_set_window_pos = true;
					break;

				case HitMinimize:
				case HitMaximize:
				case HitClose:
				default:
					break;
				}

				if (call_set_window_pos)
				{
					SetWindowPos(native_window, nullptr, info.rcWindow.left, info.rcWindow.top,
								 info.rcWindow.right - info.rcWindow.left, info.rcWindow.bottom - info.rcWindow.top, flags);
				}

				_prev_mouse_position = point;
#endif

				tim::SteadyTimestamp timestamp = tim::SteadyClock::now();
				if (timestamp - _apply_system_theme_timestamp >= tim::DblSeconds(2)) // TODO: we might want this pushed to
																					 // Window
				{
					ApplySystemThemeOnGlfw(glfw_window);
					_apply_system_theme_timestamp = timestamp;
				}
			}
		}

		virtual WindowDetailType GetDetailType() const override
		{
			return WindowDetailType::Glfw;
		}

		virtual void Show() override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (!glfw_window)
			{
				GLFWwindow* glfw_window = CreateGlfwWindow();
				GLFWwindow* expected = nullptr;
				while (!_glfw_window.compare_exchange_weak(expected, glfw_window))
					DestroyGlfwWindow(expected);

				Window::Show();
			}
			else
			{
				Focus();
			}
		}

		virtual void SetTitle(str title) override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (glfw_window)
				glfwSetWindowTitle(glfw_window, _properties.title.c_str());

			Window::SetTitle(title);
		}

		virtual void Resize(ui32 width, ui32 height) override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (glfw_window)
				glfwSetWindowSize(glfw_window, (i32)width, (i32)height);

			Window::Resize(width, height);
		}

		virtual void Minimize() override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (glfw_window && !glfwGetWindowAttrib(glfw_window, GLFW_ICONIFIED))
				glfwIconifyWindow(glfw_window);

			Window::Minimize();
		}

		virtual void RestoreFromMinimize() override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (glfw_window && glfwGetWindowAttrib(glfw_window, GLFW_ICONIFIED))
				glfwRestoreWindow(glfw_window);

			Window::RestoreFromMinimize();
		}

		virtual void Maximize() override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);

#if NP_ENGINE_PLATFORM_IS_WINDOWS
			GetWindowPlacement((HWND)GetNativeFromGlfw(glfw_window), &_prev_window_placement);
#endif

			if (glfw_window && !glfwGetWindowAttrib(glfw_window, GLFW_MAXIMIZED))
				glfwMaximizeWindow(glfw_window);

			Window::Maximize();
		}

		virtual void RestoreFromMaximize() override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (glfw_window && glfwGetWindowAttrib(glfw_window, GLFW_MAXIMIZED))
				glfwRestoreWindow(glfw_window);

			Window::RestoreFromMaximize();
		}

		virtual bl IsMinimized() const override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			return glfw_window && glfwGetWindowAttrib(glfw_window, GLFW_ICONIFIED);
		}

		virtual bl IsMaximized() const override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			return glfw_window && glfwGetWindowAttrib(glfw_window, GLFW_MAXIMIZED);
		}

		virtual void Focus() override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (glfw_window && !glfwGetWindowAttrib(glfw_window, GLFW_FOCUSED))
				glfwFocusWindow(glfw_window);

			Window::Focus();
		}

		virtual bl IsFocused() const override
		{
			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			return glfw_window && glfwGetWindowAttrib(glfw_window, GLFW_FOCUSED);
		}

		virtual ::glm::uvec2 GetFramebufferSize() override
		{
			::glm::uvec2 size{};
			i32 width = 0, height = 0;

			GLFWwindow* glfw_window = _glfw_window.load(mo_acquire);
			if (glfw_window)
				glfwGetFramebufferSize(glfw_window, &width, &height);

			size.x = width;
			size.y = height;
			return size;
		}

		virtual void* GetDetailWindow() const override
		{
			return _glfw_window.load(mo_acquire);
		}

		virtual void* GetNativeWindow() const override
		{
			void* native_window = nullptr;

#if NP_ENGINE_PLATFORM_IS_APPLE
				// TODO: GetNativeWindow for apple

#elif NP_ENGINE_PLATFORM_IS_LINUX
				// TODO: GetNativeWindow for linux

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
			native_window = GetNativeFromGlfw(_glfw_window.load(mo_acquire));

#endif
			return native_window;
		}
	};
} // namespace np::win::__detail

#endif /* NP_ENGINE_GLFW_WINDOW_HPP */