#include <NP-Engine/NP-Engine.hpp>

#include <iostream>

namespace np
{
    namespace app
    {
        class GameApp : public Application
        {
        public:
            
            GameApp():
            Application("My Game App")
            {}
            
            ~GameApp() = default;
            
            void Run() override
            {
                ::std::cout<<"hello world from my game app!\n";
                ::std::cout<<"my title is '"<<GetWindow().GetTitle()<<"'\n";
                
                // ::np::test::TestManager manager;
                // manager.Execute();
            }
        };
        
        Application* CreateApplication(const memory::Block& main_block)
        {
            ::std::cout<<"hello world from game create application\n";
            
            return new GameApp();
        }
    }
}
