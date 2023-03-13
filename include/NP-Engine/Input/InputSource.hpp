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
		con::uset<KeyCallback> _key_callbacks;
		con::uset<MouseCallback> _mouse_callbacks;
		con::uset<MousePositionCallback> _mouse_position_callbacks;
		con::uset<ControllerCallback> _controller_callbacks;

		con::umap<void*, KeyCallback> _key_caller_callbacks;
		con::umap<void*, MouseCallback> _mouse_caller_callbacks;
		con::umap<void*, MousePositionCallback> _mouse_position_caller_callbacks;
		con::umap<void*, ControllerCallback> _controller_caller_callbacks;

		virtual void InvokeKeyCallbacks(const KeyCodeState& state)
		{
			for (auto it = _key_callbacks.begin(); it != _key_callbacks.end(); it++)
				(*it)(nullptr, state);

			for (auto it = _key_caller_callbacks.begin(); it != _key_caller_callbacks.end(); it++)
				it->second(it->first, state);
		}

		virtual void InvokeMouseCallbacks(const MouseCodeState& state)
		{
			for (auto it = _mouse_callbacks.begin(); it != _mouse_callbacks.end(); it++)
				(*it)(nullptr, state);

			for (auto it = _mouse_caller_callbacks.begin(); it != _mouse_caller_callbacks.end(); it++)
				it->second(it->first, state);
		}

		virtual void InvokeMousePositionCallbacks(const MousePosition& position)
		{
			for (auto it = _mouse_position_callbacks.begin(); it != _mouse_position_callbacks.end(); it++)
				(*it)(nullptr, position);

			for (auto it = _mouse_position_caller_callbacks.begin(); it != _mouse_position_caller_callbacks.end(); it++)
				it->second(it->first, position);
		}

		virtual void InvokeControllerCallbacks(const ControllerCodeState& state)
		{
			for (auto it = _controller_callbacks.begin(); it != _controller_callbacks.end(); it++)
				(*it)(nullptr, state);

			for (auto it = _controller_caller_callbacks.begin(); it != _controller_caller_callbacks.end(); it++)
				it->second(it->first, state);
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
				if (callback)
					_key_caller_callbacks[caller] = callback;
				else
					_key_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_key_callbacks.insert(callback);
			}
		}

		virtual void SetMouseCallback(void* caller, MouseCallback callback)
		{
			if (caller)
			{
				if (callback)
					_mouse_caller_callbacks[caller] = callback;
				else
					_mouse_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_mouse_callbacks.insert(callback);
			}
		}

		virtual void SetMousePositionCallback(void* caller, MousePositionCallback callback)
		{
			if (caller)
			{
				if (callback)
					_mouse_position_caller_callbacks[caller] = callback;
				else
					_mouse_position_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_mouse_position_callbacks.insert(callback);
			}
		}

		virtual void SetControllerCallback(void* caller, ControllerCallback callback)
		{
			if (caller)
			{
				if (callback)
					_controller_caller_callbacks[caller] = callback;
				else
					_controller_caller_callbacks.erase(caller);
			}
			else if (callback)
			{
				_controller_callbacks.insert(callback);
			}
		}

		virtual void UnsetKeyCallback(KeyCallback callback)
		{
			if (callback)
				_key_callbacks.erase(callback);
		}

		virtual void UnsetKeyCallback(void* caller)
		{
			SetKeyCallback(caller, nullptr);
		}

		virtual void UnsetMouseCallback(MouseCallback callback)
		{
			if (callback)
				_mouse_callbacks.erase(callback);
		}

		virtual void UnsetMouseCallback(void* caller)
		{
			SetMouseCallback(caller, nullptr);
		}

		virtual void UnsetMousePositionCallback(MousePositionCallback callback)
		{
			if (callback)
				_mouse_position_callbacks.erase(callback);
		}

		virtual void UnsetMousePositionCallback(void* caller)
		{
			SetMousePositionCallback(caller, nullptr);
		}

		virtual void UnsetControllerCallback(ControllerCallback callback)
		{
			if (callback)
				_controller_callbacks.erase(callback);
		}

		virtual void UnsetControllerCallback(void* caller)
		{
			SetControllerCallback(caller, nullptr);
		}

		virtual void UnsetAllCallbacks()
		{
			_key_callbacks.clear();
			_key_caller_callbacks.clear();
			_mouse_callbacks.clear();
			_mouse_caller_callbacks.clear();
			_mouse_position_callbacks.clear();
			_mouse_position_caller_callbacks.clear();
			_controller_callbacks.clear();
			_controller_caller_callbacks.clear();
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