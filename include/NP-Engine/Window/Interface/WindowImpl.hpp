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

#include "WindowDetailType.hpp"

namespace np::win
{
	class Window : public nput::InputSource
	{
	public:
		using SizeCallback = void (*)(void* caller, ui32 width, ui32 height);
		using PositionCallback = void (*)(void* caller, i32 x, i32 y);
		using FramebufferCallback = void (*)(void* caller, ui32 width, ui32 height);
		using MinimizeCallback = void (*)(void* caller, bl minimized);
		using MaximizeCallback = void (*)(void* caller, bl maximized);
		using FocusCallback = void (*)(void* caller, bl focused);

	protected:
		const thr::Thread::Id _owning_thread_id;
		mem::sptr<srvc::Services> _services;
		mem::sptr<uid::UidHandle> _uid_handle;
		
		mutexed_wrapper<con::uset<SizeCallback>> _size_callbacks;
		mutexed_wrapper<con::uset<PositionCallback>> _position_callbacks;
		mutexed_wrapper<con::uset<FramebufferCallback>> _framebuffer_callbacks;
		mutexed_wrapper<con::uset<MinimizeCallback>> _minimize_callbacks;
		mutexed_wrapper<con::uset<MaximizeCallback>> _maximize_callbacks;
		mutexed_wrapper<con::uset<FocusCallback>> _focus_callbacks;

		mutexed_wrapper<con::umap<void*, SizeCallback>> _size_caller_callbacks;
		mutexed_wrapper<con::umap<void*, PositionCallback>> _position_caller_callbacks;
		mutexed_wrapper<con::umap<void*, FramebufferCallback>> _framebuffer_caller_callbacks;
		mutexed_wrapper<con::umap<void*, MinimizeCallback>> _minimize_caller_callbacks;
		mutexed_wrapper<con::umap<void*, MaximizeCallback>> _maximize_caller_callbacks;
		mutexed_wrapper<con::umap<void*, FocusCallback>> _focus_caller_callbacks;

		void InvokeResizeCallbacks(ui32 width, ui32 height);

		void InvokePositionCallbacks(i32 x, i32 y);

		void InvokeFramebufferCallbacks(ui32 width, ui32 height);

		void InvokeMinimizeCallbacks(bl minimized);

		void InvokeMaximizeCallbacks(bl maximized);

		void InvokeFocusCallbacks(bl focused);

		virtual bl IsOwningThread() const
		{
			return _owning_thread_id == thr::ThisThread::get_id();
		}

		Window(mem::sptr<srvc::Services> services):
			_owning_thread_id(thr::ThisThread::get_id()),
			_services(services),
			_uid_handle(_services->GetUidSystem().CreateUidHandle())
		{}

	public:
		static void Init(WindowDetailType detail_type);

		static void Terminate(WindowDetailType detail_type);

		static void Update(WindowDetailType detail_type);

		static con::vector<str> GetRequiredGfxExtentions(WindowDetailType detail_type);

		static mem::sptr<Window> Create(WindowDetailType detail_type, mem::sptr<srvc::Services> services);

		virtual ~Window() = default;

		bl operator==(const Window& other) const
		{
			return GetUid() == GetUid();
		}

		virtual void Update(tim::DblMilliseconds milliseconds) {}

		virtual void Close() = 0;

		uid::Uid GetUid() const
		{
			return _services->GetUidSystem().GetUid(_uid_handle);
		}

		virtual void SetTitle(str title) = 0;

		virtual str GetTitle() = 0;

		virtual void SetSize(ui32 width, ui32 height) = 0;

		virtual ::glm::uvec2 GetSize() = 0;

		virtual void SetPosition(i32 x, i32 y) = 0;

		virtual ::glm::ivec2 GetPosition() = 0;

		virtual void Minimize() = 0;

		virtual void RestoreFromMinimize() = 0;

		virtual void Maximize() = 0;

		virtual void RestoreFromMaximize() = 0;

		virtual bl IsMinimized() = 0;

		virtual bl IsMaximized() = 0;

		virtual void Focus() = 0;

		virtual bl IsFocused() = 0;

		virtual ::glm::uvec2 GetFramebufferSize() = 0;

		virtual WindowDetailType GetDetailType() const = 0;

		/*
			WARNING: DO NOT USE THIS UNLESS YOU KNOW WHAT YOU ARE DOING
		*/
		virtual void* GetDetailWindow() = 0;

		/*
			WARNING: DO NOT USE THIS UNLESS YOU KNOW WHAT YOU ARE DOING
		*/
		virtual void* GetNativeWindow() = 0;

		virtual void SetSizeCallback(SizeCallback callback)
		{
			SetSizeCallback(nullptr, callback);
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

		virtual void SetSizeCallback(void* caller, SizeCallback callback)
		{
			if (caller)
			{
				auto size_caller_callbacks = _size_caller_callbacks.get_access();
				if (callback)
					(*size_caller_callbacks)[caller] = callback;
				else
					size_caller_callbacks->erase(caller);
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
				auto position_caller_callbacks = _position_caller_callbacks.get_access();
				if (callback)
					(*position_caller_callbacks)[caller] = callback;
				else
					position_caller_callbacks->erase(caller);
			}
			else if (callback)
			{
				_position_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetFramebufferCallback(void* caller, FramebufferCallback callback)
		{
			if (caller)
			{
				auto framebuffer_caller_callbacks = _framebuffer_caller_callbacks.get_access();
				if (callback)
					(*framebuffer_caller_callbacks)[caller] = callback;
				else
					framebuffer_caller_callbacks->erase(caller);
			}
			else if (callback)
			{
				_framebuffer_callbacks.get_access()->insert(callback);
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

		virtual void UnsetResizeCallback(SizeCallback callback)
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

		virtual void UnsetFramebufferCallback(FramebufferCallback callback)
		{
			if (callback)
				_framebuffer_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetFramebufferCallback(void* caller)
		{
			SetFramebufferCallback(caller, nullptr);
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
			_framebuffer_callbacks.get_access()->clear();
			_minimize_callbacks.get_access()->clear();
			_maximize_callbacks.get_access()->clear();
			_focus_callbacks.get_access()->clear();
			nput::InputSource::UnsetAllCallbacks();
		}

		virtual void UnsetAllCallbacks(void* caller) override
		{
			UnsetSizeCallback(caller);
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
