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

namespace np
{
	namespace nput
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
	} // namespace nput

	static str to_str(nput::MouseCode code)
	{
		str name;

		switch (code)
		{
		case nput::MouseCode::LeftButton:
			name = "LeftButton";
			break;

		case nput::MouseCode::MiddleButton:
			name = "MiddleButton";
			break;

		case nput::MouseCode::RightButton:
			name = "RightButton";
			break;

		case nput::MouseCode::Max:
		case nput::MouseCode::Unkown:
		default:
			name = "Unkown";
			break;
		}

		return name;
	}
} // namespace np

#endif /* NP_ENGINE_MOUSE_CODE_HPP */
