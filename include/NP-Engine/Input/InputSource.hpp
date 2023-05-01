//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INPUT_SOURCE_HPP
#define NP_ENGINE_INPUT_SOURCE_HPP

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "KeyCode.hpp"
#include "MouseCode.hpp"
#include "MousePosition.hpp"
#include "ControllerCode.hpp"

namespace np::nput
{
	class InputSource
	{
	protected:
		mutexed_wrapper<con::uset<KeyCallback>> _key_callbacks;
		mutexed_wrapper<con::uset<MouseCallback>> _mouse_callbacks;
		mutexed_wrapper<con::uset<MousePositionCallback>> _mouse_position_callbacks;
		mutexed_wrapper<con::uset<ControllerCallback>> _controller_callbacks;

		mutexed_wrapper<con::umap<void*, KeyCallback>> _key_caller_callbacks;
		mutexed_wrapper<con::umap<void*, MouseCallback>> _mouse_caller_callbacks;
		mutexed_wrapper<con::umap<void*, MousePositionCallback>> _mouse_position_caller_callbacks;
		mutexed_wrapper<con::umap<void*, ControllerCallback>> _controller_caller_callbacks;

		virtual void InvokeKeyCallbacks(const KeyCodeState& state)
		{
			{
				auto callbacks = _key_callbacks.get_access();
				for (auto it = callbacks->begin(); it != callbacks->end(); it++)
					(*it)(nullptr, state);
			}
			{
				auto callbacks = _key_caller_callbacks.get_access();
				for (auto it = callbacks->begin(); it != callbacks->end(); it++)
					it->second(it->first, state);
			}
		}

		virtual void InvokeMouseCallbacks(const MouseCodeState& state)
		{
			{
				auto callbacks = _mouse_callbacks.get_access();
				for (auto it = callbacks->begin(); it != callbacks->end(); it++)
					(*it)(nullptr, state);
			}
			{
				auto callbacks = _mouse_caller_callbacks.get_access();
				for (auto it = callbacks->begin(); it != callbacks->end(); it++)
					it->second(it->first, state);
			}
		}

		virtual void InvokeMousePositionCallbacks(const MousePosition& position)
		{
			{
				auto callbacks = _mouse_position_callbacks.get_access();
				for (auto it = callbacks->begin(); it != callbacks->end(); it++)
					(*it)(nullptr, position);
			}
			{
				auto callbacks = _mouse_position_caller_callbacks.get_access();
				for (auto it = callbacks->begin(); it != callbacks->end(); it++)
					it->second(it->first, position);
			}
		}

		virtual void InvokeControllerCallbacks(const ControllerCodeState& state)
		{
			{
				auto callbacks = _controller_callbacks.get_access();
				for (auto it = callbacks->begin(); it != callbacks->end(); it++)
					(*it)(nullptr, state);
			}
			{
				auto callbacks = _controller_caller_callbacks.get_access();
				for (auto it = callbacks->begin(); it != callbacks->end(); it++)
					it->second(it->first, state);
			}
		}

	public:
		virtual void SetKeyCallback(KeyCallback callback)
		{
			SetKeyCallback(nullptr, callback);
		}

		virtual void SetMouseCallback(MouseCallback callback)
		{
			SetMouseCallback(nullptr, callback);
		}

		virtual void SetMousePositionCallback(MousePositionCallback callback)
		{
			SetMousePositionCallback(nullptr, callback);
		}

		virtual void SetControllerCallback(ControllerCallback callback)
		{
			SetControllerCallback(nullptr, callback);
		}

		virtual void SetKeyCallback(void* caller, KeyCallback callback)
		{
			if (caller)
			{
				auto key_caller_callbacks = _key_caller_callbacks.get_access();
				if (callback)
					(*key_caller_callbacks)[caller] = callback;
				else
					key_caller_callbacks->erase(caller);
			}
			else if (callback)
			{
				_key_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetMouseCallback(void* caller, MouseCallback callback)
		{
			if (caller)
			{
				auto mouse_caller_callbacks = _mouse_caller_callbacks.get_access();
				if (callback)
					(*mouse_caller_callbacks)[caller] = callback;
				else
					mouse_caller_callbacks->erase(caller);
			}
			else if (callback)
			{
				_mouse_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetMousePositionCallback(void* caller, MousePositionCallback callback)
		{
			if (caller)
			{
				auto mouse_position_caller_callbacks = _mouse_position_caller_callbacks.get_access();
				if (callback)
					(*mouse_position_caller_callbacks)[caller] = callback;
				else
					mouse_position_caller_callbacks->erase(caller);
			}
			else if (callback)
			{
				_mouse_position_callbacks.get_access()->insert(callback);
			}
		}

		virtual void SetControllerCallback(void* caller, ControllerCallback callback)
		{
			if (caller)
			{
				auto controller_caller_callbacks = _controller_caller_callbacks.get_access();
				if (callback)
					(*controller_caller_callbacks)[caller] = callback;
				else
					controller_caller_callbacks->erase(caller);
			}
			else if (callback)
			{
				_controller_callbacks.get_access()->insert(callback);
			}
		}

		virtual void UnsetKeyCallback(KeyCallback callback)
		{
			if (callback)
				_key_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetKeyCallback(void* caller)
		{
			SetKeyCallback(caller, nullptr);
		}

		virtual void UnsetMouseCallback(MouseCallback callback)
		{
			if (callback)
				_mouse_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetMouseCallback(void* caller)
		{
			SetMouseCallback(caller, nullptr);
		}

		virtual void UnsetMousePositionCallback(MousePositionCallback callback)
		{
			if (callback)
				_mouse_position_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetMousePositionCallback(void* caller)
		{
			SetMousePositionCallback(caller, nullptr);
		}

		virtual void UnsetControllerCallback(ControllerCallback callback)
		{
			if (callback)
				_controller_callbacks.get_access()->erase(callback);
		}

		virtual void UnsetControllerCallback(void* caller)
		{
			SetControllerCallback(caller, nullptr);
		}

		virtual void UnsetAllCallbacks()
		{
			_key_callbacks.get_access()->clear();
			_key_caller_callbacks.get_access()->clear();
			_mouse_callbacks.get_access()->clear();
			_mouse_caller_callbacks.get_access()->clear();
			_mouse_position_callbacks.get_access()->clear();
			_mouse_position_caller_callbacks.get_access()->clear();
			_controller_callbacks.get_access()->clear();
			_controller_caller_callbacks.get_access()->clear();
		}

		virtual void UnsetAllCallbacks(void* caller)
		{
			UnsetKeyCallback(caller);
			UnsetMouseCallback(caller);
			UnsetMousePositionCallback(caller);
			UnsetControllerCallback(caller);
		}
	};
} // namespace np::nput

#endif /* NP_ENGINE_INPUT_SOURCE_HPP */