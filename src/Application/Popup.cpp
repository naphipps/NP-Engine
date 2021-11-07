//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Application/Popup.hpp"

if NP_ENGINE_PLATFORM_IS_LINUX
#include <gtk/gtk.h>
#elif NP_ENGINE_PLATFORM_IS_WINDOWS
#include <Windows.h>
#endif

namespace np::app
{
    Popup::Select Popup::Show(str title, str message, Popup::Style style, Popup::Buttons buttons)
    {
        //TODO: implement on Windows and Linux
    }
}