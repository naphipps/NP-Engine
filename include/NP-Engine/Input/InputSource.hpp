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
		con::omap<void*, KeyCallback> _key_callbacks;
		con::omap<void*, MouseCallback> _mouse_callbacks;
		con::omap<void*, MousePositionCallback> _mouse_position_callbacks;
		con::omap<void*, ControllerCallback> _controller_callbacks;

		virtual void InvokeKeyCodeCallbacks(const KeyCodeState& state)
		{
			for (auto it = _key_callbacks.begin(); it != _key_callbacks.end(); it++)
				it->second(it->first, state);
		}

		virtual void InvokeMouseCodeCallbacks(const MouseCodeState& state)
		{
			for (auto it = _mouse_callbacks.begin(); it != _mouse_callbacks.end(); it++)
				it->second(it->first, state);
		}

		virtual void InvokeMousePositionCallbacks(const MousePosition& position)
		{
			for (auto it = _mouse_position_callbacks.begin(); it != _mouse_position_callbacks.end(); it++)
				it->second(it->first, position);
		}

		virtual void InvokeControllerCodeCallbacks(const ControllerCodeState& state)
		{
			for (auto it = _controller_callbacks.begin(); it != _controller_callbacks.end(); it++)
				it->second(it->first, state);
		}

	public:

		virtual void SetKeyCodeCallback(void* caller, KeyCallback callback)
		{
			if (callback)
				_key_callbacks[caller] = callback;
			else
				_key_callbacks.erase(caller);
		}

		virtual void SetMouseCodeCallback(void* caller, MouseCallback callback)
		{
			if (callback)
				_mouse_callbacks[caller] = callback;
			else
				_mouse_callbacks.erase(caller);
		}

		virtual void SetMousePositionCallback(void* caller, MousePositionCallback callback)
		{
			if (callback)
				_mouse_position_callbacks[caller] = callback;
			else
				_mouse_position_callbacks.erase(caller);
		}

		virtual void SetControllerCodeCallback(void* caller, ControllerCallback callback)
		{
			if (callback)
				_controller_callbacks[caller] = callback;
			else
				_controller_callbacks.erase(caller);
		}

		virtual void UnsetAllCallbacks(void* caller)
		{
			SetKeyCodeCallback(caller, nullptr);
			SetMouseCodeCallback(caller, nullptr);
			SetMousePositionCallback(caller, nullptr);
			SetControllerCodeCallback(caller, nullptr);
		}
	};
}

#endif /* NP_ENGINE_INPUT_SOURCE_HPP */