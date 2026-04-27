//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Window/Interface/Popup.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX
	#include <gtk/gtk.h>

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
// clang-format off
	#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
	// clang-format on
	#include <Windows.h>

#else
	#error This file should only be compiled on Linux or Windows machines

#endif

namespace np::win
{
	PopupSelection Popup::Show(mem::sptr<win::Window> parent, str title, str message, PopupStyle style, PopupButtons buttons)
	{
		PopupSelection select = PopupSelection::Error;

#if NP_ENGINE_PLATFORM_IS_LINUX
		if (gtk_init_check(0, nullptr))
		{
			GtkWidget* parent = gtk_window_new(GTK_WINDOW_TOPLEVEL);

			GtkMessageType gtkstyle = GTK_MESSAGE_INFO;
			switch (style)
			{
			case PopupStyle::Info:
				gtkstyle = GTK_MESSAGE_INFO;
				break;
			case PopupStyle::Warning:
				gtkstyle = GTK_MESSAGE_WARNING;
				break;
			case PopupStyle::Error:
				gtkstyle = GTK_MESSAGE_ERROR;
				break;
			case PopupStyle::Question:
				gtkstyle = GTK_MESSAGE_QUESTION;
				break;
			}

			GtkButtonsType gtkbuttons = GTK_BUTTONS_OK;
			switch (buttons)
			{
			case PopupButtons::Ok:
				gtkbuttons = GTK_BUTTONS_OK;
				break;
			case PopupButtons::Ok | PopupButtons::Cancel:
				gtkbuttons = GTK_BUTTONS_OK_CANCEL;
				break;
			case PopupButtons::Yes | PopupButtons::No:
				gtkbuttons = GTK_BUTTONS_YES_NO;
				break;
			case PopupButtons::Quit:
				gtkbuttons = GTK_BUTTONS_CLOSE;
				break;
			}

			GtkWidget* dialog =
				gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_MODAL, gtkstyle, gtkbuttons, "%s", message.c_str());

			gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());

			switch (gtk_dialog_run(GTK_DIALOG(dialog)))
			{
			case GTK_RESPONSE_OK:
				select = PopupSelection::Ok;
				break;
			case GTK_RESPONSE_CANCEL:
				select = PopupSelection::Cancel;
				break;
			case GTK_RESPONSE_YES:
				select = PopupSelection::Yes;
				break;
			case GTK_RESPONSE_NO:
				select = PopupSelection::No;
				break;
			case GTK_RESPONSE_CLOSE:
				select = PopupSelection::Quit;
				break;
			default:
				select = PopupSelection::None;
				break;
			}

			gtk_widget_destroy(GTK_WIDGET(dialog));
			gtk_widget_destroy(GTK_WIDGET(parent));
			while (g_main_context_iteration(nullptr, false))
				;
		}

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
		UINT flags = MB_TASKMODAL | MB_TOPMOST;

		switch (style)
		{
		case PopupStyle::Warning:
			flags |= MB_ICONWARNING;
			break;

		case PopupStyle::Error:
			flags |= MB_ICONERROR;
			break;

		case PopupStyle::Question:
			flags |= MB_ICONQUESTION;
			break;

		case PopupStyle::Info:
		default:
			flags |= MB_ICONINFORMATION;
			break;
		}

		switch (buttons)
		{
		case PopupButtons::Ok | PopupButtons::Cancel:
			flags |= MB_OKCANCEL;
			break;

		case PopupButtons::Yes | PopupButtons::No:
			flags |= MB_YESNO;
			break;

		case PopupButtons::Yes | PopupButtons::No | PopupButtons::Cancel:
			flags |= MB_YESNOCANCEL;
			break;

		case PopupButtons::Ok:
		case PopupButtons::Quit:
		default:
			flags |= MB_OK;
			break;
		}

		HWND hwnd = parent ? (HWND)parent->GetNativeWindow() : nullptr;

		switch (MessageBox(hwnd, message.c_str(), title.c_str(), flags))
		{
		case IDOK:
			select = buttons == PopupButtons::Quit ? PopupSelection::Quit : PopupSelection::Ok;
			break;

		case IDCANCEL:
			select = PopupSelection::Cancel;
			break;

		case IDYES:
			select = PopupSelection::Yes;
			break;

		case IDNO:
			select = PopupSelection::No;
			break;

		default:
			select = PopupSelection::None;
			break;
		}

#endif

		return select;
	}
} // namespace np::app