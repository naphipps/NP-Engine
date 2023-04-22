//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/2/21
//
//##===----------------------------------------------------------------------===##//

//TODO: I think our test app should change working dir
//TODO: I think our test app should contain all assets, including shaders

#include "NP-Engine-Tester.hpp"

namespace np::app
{
	mem::sptr<Application> CreateApplication(::np::mem::sptr<::np::srvc::Services> services)
	{
		NP_ENGINE_PROFILE_FUNCTION();
		NP_ENGINE_LOG_INFO("Hello world from game create application.");
		return mem::create_sptr<GameApp>(services->GetAllocator(), services);
	}
} // namespace np::app
