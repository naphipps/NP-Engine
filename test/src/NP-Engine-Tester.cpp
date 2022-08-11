//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/2/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine-Tester.hpp"

namespace np::app
{
	Application* CreateApplication(memory::Allocator& application_allocator)
	{
		::std::cout << "hello world from game create application\n";
		return memory::Create<GameApp>(application_allocator, application_allocator);
	}
} // namespace np::app
