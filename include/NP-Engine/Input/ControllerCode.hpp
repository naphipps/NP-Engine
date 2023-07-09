//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CONTROLLER_CODE_HPP
#define NP_ENGINE_CONTROLLER_CODE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "InputState.hpp"

namespace np
{
	namespace nput
	{
		enum class ControllerCode : ui32
		{
			Unkown = 0,

			Start,
			Select,
			Home,

			RightTrigger,
			RightBumper,
			RightStickButton,

			RightPadUp,
			RightPadRight,
			RightPadDown,
			RightPadLeft,

			LeftTrigger,
			LeftBumper,
			LeftStickButton,

			LeftPadUp,
			LeftPadRight,
			LeftPadDown,
			LeftPadLeft,

			Max
		};

		using ControllerCodeState = InputState<ControllerCode>;
		using ControllerCodeStates = InputStates<ControllerCode, (siz)ControllerCode::Max>;
		using ControllerCallback = void (*)(void* caller, const nput::ControllerCodeState& controller_state);
	} // namespace nput

	static str to_str(nput::ControllerCode code)
	{
		str name;

		switch (code)
		{
		case nput::ControllerCode::Start:
			name = "Start";
			break;

		case nput::ControllerCode::Select:
			name = "Select";
			break;

		case nput::ControllerCode::Home:
			name = "Home";
			break;

		case nput::ControllerCode::RightTrigger:
			name = "RightTrigger";
			break;

		case nput::ControllerCode::RightBumper:
			name = "RightBumper";
			break;

		case nput::ControllerCode::RightStickButton:
			name = "RightStickButton";
			break;

		case nput::ControllerCode::RightPadUp:
			name = "RightPadUp";
			break;

		case nput::ControllerCode::RightPadRight:
			name = "RightPadRight";
			break;

		case nput::ControllerCode::RightPadDown:
			name = "RightPadDown";
			break;

		case nput::ControllerCode::RightPadLeft:
			name = "RightPadLeft";
			break;

		case nput::ControllerCode::LeftTrigger:
			name = "LeftTrigger";
			break;

		case nput::ControllerCode::LeftBumper:
			name = "LeftBumper";
			break;

		case nput::ControllerCode::LeftStickButton:
			name = "LeftStickButton";
			break;

		case nput::ControllerCode::LeftPadUp:
			name = "LeftPadUp";
			break;

		case nput::ControllerCode::LeftPadRight:
			name = "LeftPadRight";
			break;

		case nput::ControllerCode::LeftPadDown:
			name = "LeftPadDown";
			break;

		case nput::ControllerCode::LeftPadLeft:
			name = "LeftPadLeft";
			break;

		case nput::ControllerCode::Max:
		case nput::ControllerCode::Unkown:
		default:
			name = "Unkown";
			break;
		}

		return name;
	}
} // namespace np

#endif /* NP_ENGINE_CONTROLLER_CODE_HPP */