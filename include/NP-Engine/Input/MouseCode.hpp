//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MOUSE_CODE_HPP
#define NP_ENGINE_MOUSE_CODE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

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

	static str GetMouseCodeName(MouseCode code)
	{
		str name;

		switch (code)
		{
		case MouseCode::LeftButton:
			name = "LeftButton";
			break;

		case MouseCode::MiddleButton:
			name = "MiddleButton";
			break;

		case MouseCode::RightButton:
			name = "RightButton";
			break;

		case MouseCode::Max:
		case MouseCode::Unkown:
		default:
			name = "Unkown";
			break;
		}

		return name;
	}
} // namespace np::nput

#endif /* NP_ENGINE_MOUSE_CODE_HPP */
