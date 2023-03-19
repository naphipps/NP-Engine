//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INPUT_LISTENER_HPP
#define NP_ENGINE_INPUT_LISTENER_HPP

#include "KeyCode.hpp"
#include "MouseCode.hpp"
#include "MousePosition.hpp"
#include "ControllerCode.hpp"

namespace np::nput
{
	class InputListener
	{
	public:
		static void SubmitKeyState(void* caller, const KeyCodeState& key_code_state)
		{
			((InputListener*)caller)->Submit(key_code_state);
		}

		static void SubmitMouseState(void* caller, const MouseCodeState& mouse_code_state)
		{
			((InputListener*)caller)->Submit(mouse_code_state);
		}

		static void SubmitMousePosition(void* caller, const MousePosition& mouse_position)
		{
			((InputListener*)caller)->Submit(mouse_position);
		}

		static void SubmitControllerState(void* caller, const ControllerCodeState& controller_code_state)
		{
			((InputListener*)caller)->Submit(controller_code_state);
		}

		virtual void Submit(const KeyCodeState& key_code_state) = 0;

		virtual void Submit(const MouseCodeState& mouse_code_state) = 0;

		virtual void Submit(const ControllerCodeState& controller_code_state) = 0;

		virtual void Submit(const MousePosition& mouse_position) = 0;
	};
} // namespace np::nput

#endif /* NP_ENGINE_INPUT_LISTENER_HPP */