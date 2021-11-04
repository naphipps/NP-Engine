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

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/Platform/Windows/Application/WindowsWindow.hpp"

#endif

namespace np
{
    namespace app
    {
        Window* CreateWindow(memory::Allocator& allocator, const Window::Properties& properties)
        {
#if NP_ENGINE_PLATFORM_IS_APPLE
            memory::Block block = allocator.Allocate(sizeof(AppleWindow));
            memory::Construct<AppleWindow>(block, properties);
            return static_cast<AppleWindow*>(block.ptr);

#elif NP_ENGINE_PLATFORM_IS_LINUX 
            memory::Block block = allocator.Allocate(sizeof(LinuxWindow));
            memory::Construct<LinuxWindow>(block, properties);
            return static_cast<LinuxWindow*>(block.ptr);

#elif NP_ENGINE_PLATFORM_IS_WINDOWS 
            memory::Block block = allocator.Allocate(sizeof(WindowsWindow));
            memory::Construct<WindowsWindow>(block, properties);
            return static_cast<WindowsWindow*>(block.ptr);

#else
#error We do not support a window for this platform
#endif
        }
    }
}
