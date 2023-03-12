//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_IMPL_HPP
#define NP_ENGINE_WINDOW_IMPL_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "WindowDetailType.hpp"

#ifndef NP_ENGINE_WINDOW_LOOP_DURATION
	#define NP_ENGINE_WINDOW_LOOP_DURATION 0 // milliseconds
#endif

namespace np::win
{
	class Window : public nput::InputSource
	{
	public:
		constexpr static ui32 DEFAULT_WIDTH = 800;
		constexpr static ui32 DEFAULT_HEIGHT = 600;

		struct Properties
		{
			str title = "NP Window";
			ui32 width = DEFAULT_WIDTH;
			ui32 height = DEFAULT_HEIGHT;
		};

		using ResizeCallback = void (*)(void* caller, ui32 width, ui32 height);
		using PositionCallback = void (*)(void* caller, i32 x, i32 y);
		using FramebufferCallback = void(*)(void* caller, ui32 width, ui32 height);
		using MinimizeCallback = void(*)(void* caller, bl minimized);
		using MaximizeCallback = void(*)(void* caller, bl maximized);
		using FocusCallback = void(*)(void* caller, bl focused);

	protected:
		Properties _properties;
		srvc::Services& _services;
		thr::Thread _thread;
		atm_bl _show_procedure_is_complete;

		con::uset<ResizeCallback> _resize_callbacks;
		con::uset<PositionCallback> _position_callbacks;
		con::uset<FramebufferCallback> _framebuffer_callbacks;
		con::uset<MinimizeCallback> _minimize_callbacks;
		con::uset<MaximizeCallback> _maximize_callbacks;
		con::uset<FocusCallback> _focus_callbacks;

		con::umap<void*, ResizeCallback> _resize_caller_callbacks;
		con::umap<void*, PositionCallback> _position_caller_callbacks;
		con::umap<void*, FramebufferCallback> _framebuffer_caller_callbacks;
		con::umap<void*, MinimizeCallback> _minimize_caller_callbacks;
		con::umap<void*, MaximizeCallback> _maximize_caller_callbacks;
		con::umap<void*, FocusCallback> _focus_caller_callbacks;

		void InvokeResizeCallbacks(ui32 width, ui32 height);

		void InvokePositionCallbacks(i32 x, i32 y);

		void InvokeFramebufferCallbacks(ui32 width, ui32 height);

		void InvokeMinimizeCallbacks(bl minimized);

		void InvokeMaximizeCallbacks(bl maximized);

		void InvokeFocusCallbacks(bl focused);

		virtual void ShowProcedure();

		static void ClosingCallback(void* caller, mem::Delegate& d)
		{
			((Window*)caller)->ClosingProcedure(d);
		}

		virtual void ClosingProcedure(mem::Delegate& d);

		virtual void DetailShowProcedure() = 0;

		virtual void DetailCloseProcedure() = 0;

		Window(Window::Properties& properties, srvc::Services& services) :
			_properties(properties),
			_services(services),
			_show_procedure_is_complete(true)
		{}

	public:

		static void Init(WindowDetailType detail_type);

		static void Terminate(WindowDetailType detail_type);

		static void Update(WindowDetailType detail_type);

		static con::vector<str> GetRequiredGfxExtentions(WindowDetailType detail_type);

		static Window* Create(WindowDetailType detail_type, srvc::Services& services, const Properties& properties);

		virtual ~Window()
		{
			_thread.Dispose();
		}
		
		virtual void Update(tim::DblMilliseconds milliseconds) {}

		virtual void Show()
		{
			_show_procedure_is_complete.store(false, mo_release);
			_thread.Run(&Window::ShowProcedure, this);
		}

		virtual void Close()
		{
			if (!_show_procedure_is_complete.load(mo_acquire))
				DetailCloseProcedure();
		}

		virtual bl IsRunning() const
		{
			return !_show_procedure_is_complete.load(mo_acquire) || _thread.IsRunning();
		}

		virtual ui32 GetWidth() const
		{
			return _properties.width;
		}

		virtual ui32 GetHeight() const
		{
			return _properties.height;
		}

		virtual str GetTitle() const
		{
			return _properties.title;
		}

		virtual Properties GetProperties() const
		{
			return _properties;
		}

		virtual void SetTitle(str title)
		{
			_properties.title = title;
		}

		virtual void Resize(ui32 width, ui32 height);

		virtual void Minimize();

		virtual void RestoreFromMinimize();

		virtual void Maximize();

		virtual void RestoreFromMaximize();

		virtual bl IsMinimized() const = 0;

		virtual bl IsMaximized() const = 0;

		virtual void Focus();

		virtual bl IsFocused() const = 0;

		virtual ::glm::uvec2 GetFramebufferSize() = 0;

		virtual WindowDetailType GetDetailType() const = 0;

		virtual void* GetDetailWindow() const = 0;

		virtual void* GetNativeWindow() const = 0;

		virtual void SetResizeCallback(ResizeCallback callback)
		{
			SetResizeCallback(nullptr, callback);
		}

		virtual void SetPositionCallback(PositionCallback callback)
		{
			SetPositionCallback(nullptr, callback);
		}

		virtual void SetFramebufferCallback(FramebufferCallback callback)
		{
			SetFramebufferCallback(nullptr, callback);
		}

		virtual void SetMinimizeCallback(MinimizeCallback callback)
		{
			SetMinimizeCallback(nullptr, callback);
		}

		virtual void SetMaximizeCallback(MaximizeCallback callback)
		{
			SetMaximizeCallback(nullptr, callback);
		}

		virtual void SetFocusCallback(FocusCallback callback)
		{
			SetFocusCallback(nullptr, callback);
		}

		virtual void SetResizeCallback(void* caller, ResizeCallback callback)
		{
			if (caller)
			{
				if (callback)
					_resize_caller_callbacks[caller] = callback;
				else
					_resize_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_resize_callbacks.insert(callback);
			}
		}

		virtual void SetPositionCallback(void* caller, PositionCallback callback)
		{
			if (caller)
			{
				if (callback)
					_position_caller_callbacks[caller] = callback;
				else
					_position_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_position_callbacks.insert(callback);
			}
		}
		
		virtual void SetFramebufferCallback(void* caller, FramebufferCallback callback)
		{
			if (caller)
			{
				if (callback)
					_framebuffer_caller_callbacks[caller] = callback;
				else
					_framebuffer_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_framebuffer_callbacks.insert(callback);
			}
		}

		virtual void SetMinimizeCallback(void* caller, MinimizeCallback callback)
		{
			if (caller)
			{
				if (callback)
					_minimize_caller_callbacks[caller] = callback;
				else
					_minimize_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_minimize_callbacks.insert(callback);
			}
		}

		virtual void SetMaximizeCallback(void* caller, MaximizeCallback callback)
		{
			if (caller)
			{
				if (callback)
					_maximize_caller_callbacks[caller] = callback;
				else
					_maximize_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_maximize_callbacks.insert(callback);
			}
		}

		virtual void SetFocusCallback(void* caller, FocusCallback callback)
		{
			if (caller)
			{
				if (callback)
					_focus_caller_callbacks[caller] = callback;
				else
					_focus_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_focus_callbacks.insert(callback);
			}
		}

		virtual void UnsetResizeCallback(ResizeCallback callback)
		{
			if (callback)
				_resize_callbacks.erase(callback);
		}

		virtual void UnsetResizeCallback(void* caller)
		{
			SetResizeCallback(caller, nullptr);
		}

		virtual void UnsetPositionCallback(PositionCallback callback)
		{
			if (callback)
				_position_callbacks.erase(callback);
		}

		virtual void UnsetPositionCallback(void* caller)
		{
			SetPositionCallback(caller, nullptr);
		}

		virtual void UnsetFramebufferCallback(FramebufferCallback callback)
		{
			if (callback)
				_framebuffer_callbacks.erase(callback);
		}

		virtual void UnsetFramebufferCallback(void* caller)
		{
			SetFramebufferCallback(caller, nullptr);
		}

		virtual void UnsetMinimizeCallback(MinimizeCallback callback)
		{
			if (callback)
				_minimize_callbacks.erase(callback);
		}

		virtual void UnsetMinimizeCallback(void* caller)
		{
			SetMinimizeCallback(caller, nullptr);
		}

		virtual void UnsetMaximizeCallback(MaximizeCallback callback)
		{
			if (callback)
				_maximize_callbacks.erase(callback);
		}

		virtual void UnsetMaximizeCallback(void* caller)
		{
			SetMaximizeCallback(caller, nullptr);
		}

		virtual void UnsetFocusCallback(FocusCallback callback)
		{
			if (callback)
				_focus_callbacks.erase(callback);
		}

		virtual void UnsetFocusCallback(void* caller)
		{
			SetFocusCallback(caller, nullptr);
		}

		virtual void UnsetAllCallbacks() override
		{
			_resize_callbacks.clear();
			_position_callbacks.clear();
			_framebuffer_callbacks.clear();
			_minimize_callbacks.clear();
			_maximize_callbacks.clear();
			_focus_callbacks.clear();
			nput::InputSource::UnsetAllCallbacks();
		}

		virtual void UnsetAllCallbacks(void* caller) override
		{
			UnsetResizeCallback(caller);
			UnsetPositionCallback(caller);
			UnsetFramebufferCallback(caller);
			UnsetMinimizeCallback(caller);
			UnsetMaximizeCallback(caller);
			UnsetFocusCallback(caller);
			nput::InputSource::UnsetAllCallbacks(caller);
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_IMPL_HPP */
