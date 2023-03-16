//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/2/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine-Tester.hpp"

namespace np::app
{
	mem::sptr<Application> CreateApplication(::np::srvc::Services& services)
	{
		NP_ENGINE_LOG_INFO("Hello world from game create application.");
		return mem::CreateSptr<GameApp>(services.GetAllocator(), services);
	}
} // namespace np::app
