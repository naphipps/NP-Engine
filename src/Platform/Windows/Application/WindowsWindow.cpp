//
//  WindowsWindow.cpp
//  NP-Engine
//
//  Created by Nathan Phipps on 11/2/21.
//

#include "NP-Engine/Platform/Windows/Application/WindowsWindow.hpp"

namespace np::app
{
	ui32 WindowsWindow::_window_count = 0;
	Mutex WindowsWindow::_mutex;
}