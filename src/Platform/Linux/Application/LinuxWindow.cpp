//
//  LinuxWindow.cpp
//  NP-Engine
//
//  Created by Nathan Phipps on 11/2/21.
//

#include "NP-Engine/Platform/Linux/Application/LinuxWindow.hpp"

namespace np::app
{
	ui32 LinuxWindow::_window_count = 0;
	Mutex LinuxWindow::_mutex;
}