//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/14/22
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Input/InputSource.hpp"
#include "NP-Engine/Input/InputListener.hpp"

namespace np::nput
{
	void InputSource::SetListener(InputListener* listener)
	{
		SetKeyCodeCallback(listener, InputListener::SubmitKeyCodeState);
		SetMouseCodeCallback(listener, InputListener::SubmitMouseCodeState);
		SetMousePositionCallback(listener, InputListener::SubmitMousePosition);
		SetControllerCodeCallback(listener, InputListener::SubmitControllerCodeState);
	}
}