//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/2/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine-Tester.hpp"

namespace np::app
{
	Application* CreateApplication(::np::srvc::Services& app_services)
	{
		NP_ENGINE_LOG_INFO("Hello world from game create application.");
		return mem::Create<GameApp>(app_services.GetAllocator(), app_services);
	}
} // namespace np::app
