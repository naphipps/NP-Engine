//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/10/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_IMPL_HPP
#define NP_ENGINE_WINDOW_IMPL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

#include "DetailType.hpp"

// TODO: add a way to create fixed-size windows, fullscreen, frameless, etc
/*
	TODO: add support for parent / child windows
		<https://stackoverflow.com/questions/46152212/embed-windowglfwcreatewindow-as-child-to-c-mfc-parent-form>
*/
/*
	TODO: support all this:
		<https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/ui/apply-windows-themes>
*/

namespace np::win
{
	class Window : public nput::InputSource
	{
	public:
		using PreventCloseCallback = bl(*)(void* caller);
		using SizeCallback = void (*)(void* caller, ::glm::uvec2 size);
		using PositionCallback = void (*)(void* caller, ::glm::ivec2 position);
		using FramebufferSizeCallback = void (*)(void* caller, ::glm::uvec2 framebuffer_size);
		using MinimizeCallback = void (*)(void* caller, bl minimized);
		using MaximizeCallback = void (*)(void* caller, bl maximized);
		using FocusCallback = void (*)(void* caller, bl focused);

	protected:
		const thr::thread::id _owning_thread_id;
		mem::sptr<srvc::Services> _services;
		const uid::Uid _id;

		mutexed_wrapper<con::uset<PreventCloseCallback>> _prevent_close_callbacks;
		mutexed_wrapper<con::uset<SizeCallback>> _size_callbacks;
		mutexed_wrapper<con::uset<PositionCallback>> _position_callbacks;
		mutexed_wrapper<con::uset<FramebufferSizeCallback>> _framebuffer_size_callbacks;
		mutexed_wrapper<con::uset<MinimizeCallback>> _minimize_callbacks;
		mutexed_wrapper<con::uset<MaximizeCallback>> _maximize_callbacks;
		mutexed_wrapper<con::uset<FocusCallback>> _focus_callbacks;

		mutexed_wrapper<con::umap<void*, PreventCloseCallback>> _prevent_close_caller_callbacks;
		mutexed_wrapper<con::umap<void*, SizeCallback>> _size_caller_callbacks;
		mutexed_wrapper<con::umap<void*, PositionCallback>> _position_caller_callbacks;
		mutexed_wrapper<con::umap<void*, FramebufferSizeCallback>> _framebuffer_size_caller_callbacks;
		mutexed_wrapper<con::umap<void*, MinimizeCallback>> _minimize_caller_callbacks;
		mutexed_wrapper<con::umap<void*, MaximizeCallback>> _maximize_caller_callbacks;
		mutexed_wrapper<con::umap<void*, FocusCallback>> _focus_caller_callbacks;

		bl InvokePreventCloseCallbacks();

		void InvokeSizeCallbacks(::glm::uvec2 size);

		void InvokePositionCallbacks(::glm::ivec2 position);

		void InvokeFramebufferSizeCallbacks(::glm::uvec2 framebuffer_size);
		//TODO: ^ these callbacks should be renamed to "FramebufferSize" instead of just "Framebuffer"

		void InvokeMinimizeCallbacks(bl minimized);

		void InvokeMaximizeCallbacks(bl maximized);

		void InvokeFocusCallbacks(bl focused);

		virtual bl IsOwningThread() const
		{
			return _owning_thread_id == thr::this_thread::get_id();
		}

		Window(mem::sptr<srvc::Services> services, uid::Uid id):
			_owning_thread_id(thr::this_thread::get_id()),
			_services(services),
			_id(id)
		{}

	public:
		static void Init(DetailType detail_type);

		static void Terminate(DetailType detail_type);

		static void Update(DetailType detail_type);

		static con::vector<str> GetRequiredGpuExtentions(DetailType detail_type);

		static mem::sptr<Window> Create(DetailType detail_type, mem::sptr<srvc::Services> services, uid::Uid id);

		virtual ~Window() = default;

		bl operator==(const Window& other) const
		{
			return GetUid() == GetUid();
		}

		virtual void Update(tim::milliseconds m) {}

		virtual void Close() = 0;

		uid::Uid GetUid() const
		{
			return _id;
		}

		virtual void SetTitle(str title) = 0;

		virtual str GetTitle() = 0;

		virtual void SetSize(::glm::uvec2 size) = 0;

		virtual ::glm::uvec2 GetSize() = 0;

		virtual void SetPosition(::glm::ivec2 position) = 0;

		virtual ::glm::ivec2 GetPosition() = 0;

		virtual void Minimize() = 0;

		virtual void RestoreFromMinimize() = 0;

		virtual void Maximize() = 0;

		virtual void RestoreFromMaximize() = 0;

		virtual bl IsSnapped() = 0;

		virtual bl IsMinimized() = 0;

		virtual bl IsMaximized() = 0;

		virtual void Focus() = 0;

		virtual bl IsFocused() = 0;

		virtual ::glm::uvec2 GetFramebufferSize() = 0;

		virtual DetailType GetDetailType() const = 0;

		/*
			WARNING: DO NOT USE THIS UNLESS YOU KNOW WHAT YOU ARE DOING
		*/
		virtual void* GetDetailWindow() = 0;

		/*
			WARNING: DO NOT USE THIS UNLESS YOU KNOW WHAT YOU ARE DOING
		*/
		virtual void* GetNativeWindow() = 0;

		virtual void SetPreventCloseCallback(PreventCloseCallback callback)
		{
			SetPreventCloseCallback(nullptr, callback);
		}

		virtual void SetSizeCallback(SizeCallback callback)
		{
			SetSizeCallback(nullptr, callback);
		}

		virtual void SetPositionCallback(PositionCallback callback)
		{
			SetPositionCallback(nullptr, callback);
		}

		virtual void SetFramebufferSizeCallback(FramebufferSizeCallback callback)
		{
			SetFramebufferSizeCallback(nullptr, callback);
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

		virtual void SetPreventCloseCallback(void* caller, PreventCloseCallback callback)
		{
			if (caller)
			{
				auto callbacks = _prevent_close_caller_callbacks.get_access();
				if (callback)
					(*callbacks)[caller] = callback;
				else
					callbacks->erase(caller);
			}
			else if (callback)
			{
				_prevent_close_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetSizeCallback(void* caller, SizeCallback callback)
		{
			if (caller)
			{
				auto callbacks = _size_caller_callbacks.get_access();
				if (callback)
					(*callbacks)[caller] = callback;
				else
					callbacks->erase(caller);
			}
			else if (callback)
			{
				_size_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetPositionCallback(void* caller, PositionCallback callback)
		{
			if (caller)
			{
				auto callbacks = _position_caller_callbacks.get_access();
				if (callback)
					(*callbacks)[caller] = callback;
				else
					callbacks->erase(caller);
			}
			else if (callback)
			{
				_position_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetFramebufferSizeCallback(void* caller, FramebufferSizeCallback callback)
		{
			if (caller)
			{
				auto callbacks = _framebuffer_size_caller_callbacks.get_access();
				if (callback)
					(*callbacks)[caller] = callback;
				else
					callbacks->erase(caller);
			}
			else if (callback)
			{
				_framebuffer_size_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetMinimizeCallback(void* caller, MinimizeCallback callback)
		{
			if (caller)
			{
				auto minimize_caller_callbacks = _minimize_caller_callbacks.get_access();
				if (callback)
					(*minimize_caller_callbacks)[caller] = callback;
				else
					minimize_caller_callbacks->erase(caller);
			}
			else if (callback)
			{
				_minimize_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetMaximizeCallback(void* caller, MaximizeCallback callback)
		{
			if (caller)
			{
				auto maximize_caller_callbacks = _maximize_caller_callbacks.get_access();
				if (callback)
					(*maximize_caller_callbacks)[caller] = callback;
				else
					maximize_caller_callbacks->erase(caller);
			}
			else if (callback)
			{
				_maximize_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetFocusCallback(void* caller, FocusCallback callback)
		{
			if (caller)
			{
				auto focus_caller_callbacks = _focus_caller_callbacks.get_access();
				if (callback)
					(*focus_caller_callbacks)[caller] = callback;
				else
					focus_caller_callbacks->erase(caller);
			}
			else if (callback)
			{
				_focus_callbacks.get_access()->insert(callback);
			}
		}

		virtual void UnsetSizeCallback(SizeCallback callback)
		{
			if (callback)
				_size_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetSizeCallback(void* caller)
		{
			SetSizeCallback(caller, nullptr);
		}

		virtual void UnsetPositionCallback(PositionCallback callback)
		{
			if (callback)
				_position_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetPositionCallback(void* caller)
		{
			SetPositionCallback(caller, nullptr);
		}

		virtual void UnsetFramebufferSizeCallback(FramebufferSizeCallback callback)
		{
			if (callback)
				_framebuffer_size_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetFramebufferSizeCallback(void* caller)
		{
			SetFramebufferSizeCallback(caller, nullptr);
		}

		virtual void UnsetMinimizeCallback(MinimizeCallback callback)
		{
			if (callback)
				_minimize_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetMinimizeCallback(void* caller)
		{
			SetMinimizeCallback(caller, nullptr);
		}

		virtual void UnsetMaximizeCallback(MaximizeCallback callback)
		{
			if (callback)
				_maximize_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetMaximizeCallback(void* caller)
		{
			SetMaximizeCallback(caller, nullptr);
		}

		virtual void UnsetFocusCallback(FocusCallback callback)
		{
			if (callback)
				_focus_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetFocusCallback(void* caller)
		{
			SetFocusCallback(caller, nullptr);
		}

		virtual void UnsetAllCallbacks() override
		{
			_size_callbacks.get_access()->clear();
			_position_callbacks.get_access()->clear();
			_framebuffer_size_callbacks.get_access()->clear();
			_minimize_callbacks.get_access()->clear();
			_maximize_callbacks.get_access()->clear();
			_focus_callbacks.get_access()->clear();
			nput::InputSource::UnsetAllCallbacks();
		}

		virtual void UnsetAllCallbacks(void* caller) override
		{
			UnsetSizeCallback(caller);
			UnsetPositionCallback(caller);
			UnsetFramebufferSizeCallback(caller);
			UnsetMinimizeCallback(caller);
			UnsetMaximizeCallback(caller);
			UnsetFocusCallback(caller);
			nput::InputSource::UnsetAllCallbacks(caller);
		}
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_IMPL_HPP */
