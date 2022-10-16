//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MOUSE_CODE_HPP
#define NP_ENGINE_MOUSE_CODE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "InputState.hpp"

namespace np::nput
{
	enum class MouseCode : ui32
	{
		Unkown = 0,
		LeftButton,
		MiddleButton,
		RightButton,
		Max
	};

	using MouseCodeState = InputState<MouseCode>;
	using MouseCodeStates = InputStates<MouseCode, (siz)MouseCode::Max>;
	using MouseCallback = void (*)(void* caller, const nput::MouseCodeState& mouse_code_state);
} // namespace np::nput

static str to_str(::np::nput::MouseCode code)
{
	str name;

	switch (code)
	{
	case ::np::nput::MouseCode::LeftButton:
		name = "LeftButton";
		break;

	case ::np::nput::MouseCode::MiddleButton:
		name = "MiddleButton";
		break;

	case ::np::nput::MouseCode::RightButton:
		name = "RightButton";
		break;

	case ::np::nput::MouseCode::Max:
	case ::np::nput::MouseCode::Unkown:
	default:
		name = "Unkown";
		break;
	}

	return name;
}

#endif /* NP_ENGINE_MOUSE_CODE_HPP */
