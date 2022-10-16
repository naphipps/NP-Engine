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

namespace np::win
{
	class Window : public nput::InputSource
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
		using FramebufferCallback = void(*)(void* caller, ui32 width, ui32 height);
		using MinimizeCallback = void(*)(void* caller, bl minimized);
		using MaximizeCallback = void(*)(void* caller, bl maximized);
		using FocusCallback = void(*)(void* caller, bl focused);

	protected:
		Properties _properties;
		srvc::Services& _services;
		thr::Thread _thread;
		atm_bl _show_procedure_is_complete;

		con::omap<void*, ResizeCallback> _resize_callbacks;
		con::omap<void*, PositionCallback> _position_callbacks;
		con::omap<void*, FramebufferCallback> _framebuffer_callbacks;
		con::omap<void*, MinimizeCallback> _minimize_callbacks;
		con::omap<void*, MaximizeCallback> _maximize_callbacks;
		con::omap<void*, FocusCallback> _focus_callbacks;

		void InvokeResizeCallbacks(ui32 width, ui32 height);

		void InvokePositionCallbacks(i32 x, i32 y);

		void InvokeFramebufferCallbacks(ui32 width, ui32 height);

		void InvokeMinimizeCallbacks(bl minimized);

		void InvokeMaximizeCallbacks(bl maximized);

		void InvokeFocusCallbacks(bl focused);

		virtual void ShowProcedure();

		virtual void DisposeThread();

		virtual void DetailShowProcedure() = 0;

		virtual void DetailCloseProcedure() = 0;

	public:

		static void Init();

		static void Terminate();

		static void Update();

		static con::vector<str> GetRequiredGfxExtentions();

		static Window* Create(srvc::Services& services, const Properties& properties);

		Window(const Window::Properties& properties, srvc::Services& services):
			_properties(properties),
			_services(services),
			_show_procedure_is_complete(true)
		{}

		virtual ~Window()
		{
			DisposeThread();
		}

		virtual void RegisterDetailType() const
		{
			__detail::RegisteredWindowDetailType.store(GetDetailType(), mo_release);
		}

		virtual void Show()
		{
			_show_procedure_is_complete.store(false, mo_release);
			_thread.Run(&Window::ShowProcedure, this);
		}

		virtual void Close()
		{
			if (_show_procedure_is_complete.load(mo_acquire))
				DisposeThread();
			else
				DetailCloseProcedure();
		}

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

		virtual void SetTitle(str title)
		{
			_properties.Title = title;
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

		virtual void SetResizeCallback(void* caller, ResizeCallback callback)
		{
			if (callback)
				_resize_callbacks[caller] = callback;
			else
				_resize_callbacks.erase(caller);
		}

		virtual void SetPositionCallback(void* caller, PositionCallback callback)
		{
			if (callback)
				_position_callbacks[caller] = callback;
			else
				_position_callbacks.erase(caller);
		}
		
		virtual void SetFramebufferCallback(void* caller, FramebufferCallback callback)
		{
			if (callback)
				_framebuffer_callbacks[caller] = callback;
			else
				_framebuffer_callbacks.erase(caller);
		}

		virtual void SetMinimizeCallback(void* caller, MinimizeCallback callback)
		{
			if (callback)
				_minimize_callbacks[caller] = callback;
			else
				_minimize_callbacks.erase(caller);
		}

		virtual void SetMaximizeCallback(void* caller, MaximizeCallback callback)
		{
			if (caller)
				_maximize_callbacks[caller] = callback;
			else
				_maximize_callbacks.erase(caller);
		}

		virtual void SetFocusCallback(void* caller, FocusCallback callback)
		{
			if (caller)
				_focus_callbacks[caller] = callback;
			else
				_focus_callbacks.erase(caller);
		}

		virtual void UnsetAllCallbacks(void* caller) override
		{
			SetResizeCallback(caller, nullptr);
			SetPositionCallback(caller, nullptr);
			SetFramebufferCallback(caller, nullptr);
			SetMinimizeCallback(caller, nullptr);
			SetMaximizeCallback(caller, nullptr);
			SetFocusCallback(caller, nullptr);
			nput::InputSource::UnsetAllCallbacks(caller);
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_IMPL_HPP */
