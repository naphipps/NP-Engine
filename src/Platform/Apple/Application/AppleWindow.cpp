//
//  AppleWindow.cpp
//  NP-Engine
//
//  Created by Nathan Phipps on 2/10/21.
//

//TODO: can we remove this?

#include "NP-Engine/Platform/Apple/Application/AppleWindow.hpp"

namespace np::app
{
    atm_ui32 AppleWindow::_window_count = 0;
    Mutex AppleWindow::_mutex;
}
