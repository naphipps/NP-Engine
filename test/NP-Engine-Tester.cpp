//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/2/21
//
//##===----------------------------------------------------------------------===##//

#include <NP-Engine/NP-Engine.hpp>

#include <iostream>

namespace np::app
{
	class GameApp : public Application
	{
	private:
		memory::Allocator& _allocator;

	public:
		GameApp(memory::Allocator& application_allocator):
			Application(Application::Properties{"My Game App"}),
			_allocator(application_allocator)
		{}

		void Run(i32 argc, chr** argv) override
		{
			::std::cout << "hello world from my game app!\n";
			::std::cout << "my title is '" << GetTitle() << "'\n";

			window::Window::Properties window_properties;
			for (siz i = 0; i < 1; i++)
			{
				window_properties.Title = "My Game Window >:D - " + to_str(i);
				window::Window* window = GetWindowLayer().CreateWindow(window_properties);
			}

			Application::Run(argc, argv);
		}
	};

	Application* CreateApplication(memory::Allocator& application_allocator)
	{
		::std::cout << "hello world from game create application\n";

		memory::Block block = application_allocator.Allocate(sizeof(GameApp));
		memory::Construct<GameApp>(block, application_allocator);
		return static_cast<GameApp*>(block.ptr);
	}
} // namespace np::app
