//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WIN_POPUP_HPP
#define NP_ENGINE_WIN_POPUP_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "WindowImpl.hpp"

//TODO: fix popups to use system themes -- this may have to be something custom

namespace np::win
{
	class PopupSelection : public enm_ui32
	{
	public:
		constexpr static ui32 Ok = BIT(0);
		constexpr static ui32 Cancel = BIT(1);
		constexpr static ui32 Yes = BIT(2);
		constexpr static ui32 No = BIT(3);
		constexpr static ui32 Quit = BIT(4);
		constexpr static ui32 Error = BIT(5);

		PopupSelection(ui32 value) : enm_ui32(value) {}
	};

	class PopupStyle : public enm_ui32
	{
	public:
		constexpr static ui32 Info = BIT(0);
		constexpr static ui32 Warning = BIT(1);
		constexpr static ui32 Error = BIT(2);
		constexpr static ui32 Question = BIT(3);

		PopupStyle(ui32 value) : enm_ui32(value) {}
	};

	class PopupButtons : public enm_ui32
	{
	public:
		constexpr static ui32 Ok = BIT(0);
		constexpr static ui32 Cancel = BIT(1);
		constexpr static ui32 Yes = BIT(2);
		constexpr static ui32 No = BIT(3);
		constexpr static ui32 Quit = BIT(4);

		PopupButtons(ui32 value) : enm_ui32(value) {}
	};

	struct Popup
	{
		static PopupSelection Show(mem::sptr<Window> parent, str title, str message, PopupStyle style, PopupButtons buttons);

		static inline PopupSelection Show(mem::sptr<Window> parent, str title, str message, PopupStyle style)
		{
			return Show(parent, title, message, style, PopupButtons::Ok);
		}

		static inline PopupSelection Show(mem::sptr<Window> parent, str title, str message, PopupButtons buttons)
		{
			return Show(parent, title, message, PopupStyle::Info, buttons);
		}

		static inline PopupSelection Show(mem::sptr<Window> parent, str title, str message)
		{
			return Show(parent, title, message, PopupStyle::Info, PopupButtons::Ok);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_WIN_POPUP_HPP */
