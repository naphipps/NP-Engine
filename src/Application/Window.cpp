//
//  Window.cpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/10/21.
//

#include "Window.hpp"

#if NP_PLATFORM_IS_APPLE
//we include AppleWindow.hpp here instead of Window.hpp to avoid a cyclic include
#include "Platform/Apple/Application/AppleWindow.hpp"
#endif

namespace np
{
    namespace app
    {
        Window* CreateWindow(const Window::Properties& properties)
        {
#if NP_PLATFORM_IS_APPLE
            return new AppleWindow(properties);
#else
#error We only support Apple for now
#endif
        }
    }
}
