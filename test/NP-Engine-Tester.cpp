#include <NP-Engine/NP-Engine.hpp>

#include <iostream>

namespace np
{
    namespace app
    {
        class GameApp : public Application
        {
        public:
            
            GameApp(memory::Allocator& application_allocator):
            Application(Application::Properties{"My Game App", application_allocator})
            {}
            
            ~GameApp() = default;
            
            void Run(i32 argc, chr** argv) override
            {
                ::std::cout<<"hello world from my game app!\n";
                ::std::cout<<"my title is '"<<GetWindow().GetTitle()<<"'\n";
            }
        };
        
        Application* CreateApplication(memory::Allocator& application_allocator)
        {
            ::std::cout << "hello world from game create application\n";

            memory::Block block = application_allocator.Allocate(sizeof(GameApp));
            memory::Construct<GameApp>(block, application_allocator);
            return static_cast<GameApp*>(block.ptr);
        }
    }
}
