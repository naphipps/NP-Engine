//
//  Window.cpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/10/21.
//

#include "NP-Engine/Application/Window.hpp"

//we include our <Platform>Window.hpp here instead of Window.hpp to avoid a cyclic include

#if NP_ENGINE_PLATFORM_IS_APPLE
#include "NP-Engine/Platform/Apple/Application/AppleWindow.hpp"

#elif NP_ENGINE_PLATFORM_IS_LINUX
#include "NP-Engine/Platform/Linux/Application/LinuxWindow.hpp"

#endif


namespace np
{
    namespace app
    {
        Window* CreateWindow(const Window::Properties& properties)
        {
#if NP_ENGINE_PLATFORM_IS_APPLE
            return new AppleWindow(properties);

#elif NP_ENGINE_PLATFORM_IS_LINUX 
            return new LinuxWindow(properties);
#else
#error We do not support a window for this platform
#endif
        }
    }
}
