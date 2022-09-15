//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_CONTROLLER_CODE_HPP
#define NP_ENGINE_CONTROLLER_CODE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

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

	static str GetControllerCodeName(ControllerCode code)
	{
		str name;

		switch (code)
		{
		case ControllerCode::Start:
			name = "Start";
			break;

		case ControllerCode::Select:
			name = "Select";
			break;

		case ControllerCode::Home:
			name = "Home";
			break;

		case ControllerCode::RightTrigger:
			name = "RightTrigger";
			break;

		case ControllerCode::RightBumper:
			name = "RightBumper";
			break;

		case ControllerCode::RightStickButton:
			name = "RightStickButton";
			break;

		case ControllerCode::RightPadUp:
			name = "RightPadUp";
			break;

		case ControllerCode::RightPadRight:
			name = "RightPadRight";
			break;

		case ControllerCode::RightPadDown:
			name = "RightPadDown";
			break;

		case ControllerCode::RightPadLeft:
			name = "RightPadLeft";
			break;

		case ControllerCode::LeftTrigger:
			name = "LeftTrigger";
			break;

		case ControllerCode::LeftBumper:
			name = "LeftBumper";
			break;

		case ControllerCode::LeftStickButton:
			name = "LeftStickButton";
			break;

		case ControllerCode::LeftPadUp:
			name = "LeftPadUp";
			break;

		case ControllerCode::LeftPadRight:
			name = "LeftPadRight";
			break;

		case ControllerCode::LeftPadDown:
			name = "LeftPadDown";
			break;

		case ControllerCode::LeftPadLeft:
			name = "LeftPadLeft";
			break;

		case ControllerCode::Max:
		case ControllerCode::Unkown:
		default:
			name = "Unkown";
			break;
		}

		return name;
	}
}

#endif /* NP_ENGINE_CONTROLLER_CODE_HPP */