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

namespace np::nput
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
} // namespace np::nput

static str to_str(::np::nput::ControllerCode code)
{
	str name;

	switch (code)
	{
	case ::np::nput::ControllerCode::Start:
		name = "Start";
		break;

	case ::np::nput::ControllerCode::Select:
		name = "Select";
		break;

	case ::np::nput::ControllerCode::Home:
		name = "Home";
		break;

	case ::np::nput::ControllerCode::RightTrigger:
		name = "RightTrigger";
		break;

	case ::np::nput::ControllerCode::RightBumper:
		name = "RightBumper";
		break;

	case ::np::nput::ControllerCode::RightStickButton:
		name = "RightStickButton";
		break;

	case ::np::nput::ControllerCode::RightPadUp:
		name = "RightPadUp";
		break;

	case ::np::nput::ControllerCode::RightPadRight:
		name = "RightPadRight";
		break;

	case ::np::nput::ControllerCode::RightPadDown:
		name = "RightPadDown";
		break;

	case ::np::nput::ControllerCode::RightPadLeft:
		name = "RightPadLeft";
		break;

	case ::np::nput::ControllerCode::LeftTrigger:
		name = "LeftTrigger";
		break;

	case ::np::nput::ControllerCode::LeftBumper:
		name = "LeftBumper";
		break;

	case ::np::nput::ControllerCode::LeftStickButton:
		name = "LeftStickButton";
		break;

	case ::np::nput::ControllerCode::LeftPadUp:
		name = "LeftPadUp";
		break;

	case ::np::nput::ControllerCode::LeftPadRight:
		name = "LeftPadRight";
		break;

	case ::np::nput::ControllerCode::LeftPadDown:
		name = "LeftPadDown";
		break;

	case ::np::nput::ControllerCode::LeftPadLeft:
		name = "LeftPadLeft";
		break;

	case ::np::nput::ControllerCode::Max:
	case ::np::nput::ControllerCode::Unkown:
	default:
		name = "Unkown";
		break;
	}

	return name;
}

#endif /* NP_ENGINE_CONTROLLER_CODE_HPP */