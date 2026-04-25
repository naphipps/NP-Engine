//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/23/26
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Application/Application.hpp"

#if NP_ENGINE_PLATFORM_IS_WINDOWS
	#include <windows.h>
#endif

namespace np::app
{
	tim::milliseconds Application::GetPlatformDefaultApplicationLoopDuration() const
	{
		tim::milliseconds duration{ NP_ENGINE_APPLICATION_LOOP_DURATION };

#if NP_ENGINE_PLATFORM_IS_WINDOWS
		duration = tim::milliseconds{ (siz)USER_TIMER_MINIMUM };
#endif

		return duration;
	}
}