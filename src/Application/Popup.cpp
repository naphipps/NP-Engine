//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Application/Popup.hpp"

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

namespace np::app
{
	Popup::Select Popup::Show(str title, str message, Popup::Style style, Popup::Buttons buttons)
	{
		Popup::Select select = Popup::Select::Error;

#if NP_ENGINE_PLATFORM_IS_LINUX
		if (gtk_init_check(0, nullptr))
		{
			GtkWidget* parent = gtk_window_new(GTK_WINDOW_TOPLEVEL);

			GtkMessageType gtkstyle = GTK_MESSAGE_INFO;
			switch (style)
			{
			case Popup::Style::Info:
				gtkstyle = GTK_MESSAGE_INFO;
				break;
			case Popup::Style::Warning:
				gtkstyle = GTK_MESSAGE_WARNING;
				break;
			case Popup::Style::Error:
				gtkstyle = GTK_MESSAGE_ERROR;
				break;
			case Popup::Style::Question:
				gtkstyle = GTK_MESSAGE_QUESTION;
				break;
			}

			GtkButtonsType gtkbuttons = GTK_BUTTONS_OK;
			switch (buttons)
			{
			case Popup::Buttons::OK:
				gtkbuttons = GTK_BUTTONS_OK;
				break;
			case Popup::Buttons::OKCancel:
				gtkbuttons = GTK_BUTTONS_OK_CANCEL;
				break;
			case Popup::Buttons::YesNo:
				gtkbuttons = GTK_BUTTONS_YES_NO;
				break;
			case Popup::Buttons::Quit:
				gtkbuttons = GTK_BUTTONS_CLOSE;
				break;
			}

			GtkWidget* dialog =
				gtk_message_dialog_new(GTK_WINDOW(parent), GTK_DIALOG_MODAL, gtkstyle, gtkbuttons, "%s", message.c_str());

			gtk_window_set_title(GTK_WINDOW(dialog), title.c_str());

			switch (gtk_dialog_run(GTK_DIALOG(dialog)))
			{
			case GTK_RESPONSE_OK:
				select = Popup::Select::OK;
				break;
			case GTK_RESPONSE_CANCEL:
				select = Popup::Select::Cancel;
				break;
			case GTK_RESPONSE_YES:
				select = Popup::Select::Yes;
				break;
			case GTK_RESPONSE_NO:
				select = Popup::Select::No;
				break;
			case GTK_RESPONSE_CLOSE:
				select = Popup::Select::Quit;
				break;
			default:
				select = Popup::Select::None;
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
		case Popup::Style::Info:
			flags |= MB_ICONINFORMATION;
			break;
		case Popup::Style::Warning:
			flags |= MB_ICONWARNING;
			break;
		case Popup::Style::Error:
			flags |= MB_ICONERROR;
			break;
		case Popup::Style::Question:
			flags |= MB_ICONQUESTION;
			break;
		default:
			flags |= MB_ICONINFORMATION;
			break;
		}

		switch (buttons)
		{
		case Popup::Buttons::OK:
		case Popup::Buttons::Quit:
			flags |= MB_OK;
			break;
		case Popup::Buttons::OKCancel:
			flags |= MB_OKCANCEL;
			break;
		case Popup::Buttons::YesNo:
			flags |= MB_YESNO;
			break;
		case Popup::Buttons::YesNoCancel:
			flags |= MB_YESNOCANCEL;
			break;
		default:
			flags |= MB_OK;
			break;
		}

		switch (MessageBox(nullptr, message.c_str(), title.c_str(), flags))
		{
		case IDOK:
			select = buttons == Popup::Buttons::Quit ? Popup::Select::Quit : Popup::Select::OK;
			break;
		case IDCANCEL:
			select = Popup::Select::Cancel;
			break;
		case IDYES:
			select = Popup::Select::Yes;
			break;
		case IDNO:
			select = Popup::Select::No;
			break;
		default:
			select = Popup::Select::None;
			break;
		}

#endif

		return select;
	}
} // namespace np::app