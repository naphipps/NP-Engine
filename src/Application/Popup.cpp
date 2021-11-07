//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Application/Popup.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX
#include <gtk/gtk.h>
#elif NP_ENGINE_PLATFORM_IS_WINDOWS
#include <Windows.h>
#endif

namespace np::app
{
    Popup::Select Popup::Show(str title, str message, Popup::Style style, Popup::Buttons buttons)
    {
#if NP_ENGINE_PLATFORM_IS_LINUX
        //TODO: implement on Linux

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
        UINT flags = MB_TASKMODAL | MB_TOPMOST;

        switch (style)
        {
        case Style::Info:
            flags |= MB_ICONINFORMATION;
            break;
        case Style::Warning:
            flags |= MB_ICONWARNING;
            break;
        case Style::Error:
            flags |= MB_ICONERROR;
            break;
        case Style::Question:
            flags |= MB_ICONQUESTION;
            break;
        default:
            flags |= MB_ICONINFORMATION;
            break;
        }

        switch (buttons)
        {
        case Buttons::OK:
        case Buttons::Quit:
            flags |= MB_OK;
            break;
        case Buttons::OKCancel:
            flags |= MB_OKCANCEL;
            break;
        case Buttons::YesNo:
            flags |= MB_YESNO;
            break;
        case Buttons::YesNoCancel:
            flags |= MB_YESNOCANCEL;
            break;
        default:
            flags |= MB_OK;
            break;
        }

        int index = MessageBox(nullptr, message.c_str(), title.c_str(), flags);
        Select select = Select::None;

        switch (index) {
        case IDOK:
            select = buttons == Buttons::Quit ? Select::Quit : Select::OK;
            break;
        case IDCANCEL:
            select = Select::Cancel;
            break;
        case IDYES:
            select = Select::Yes;
            break;
        case IDNO:
            select = Select::No;
            break;
        }

        return select;
#endif
    }
}