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

		memory::Block block = application_allocator.Allocate(sizeof(GameApp));
		memory::Construct<GameApp>(block, application_allocator);
		return static_cast<GameApp*>(block.ptr);
	}
} // namespace np::app
