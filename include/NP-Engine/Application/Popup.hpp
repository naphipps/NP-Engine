//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_POPUP_HPP
#define NP_ENGINE_POPUP_HPP

#include <string>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Insight/Insight.hpp"

namespace np::app
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
		static PopupSelection Show(::std::string title, ::std::string message, PopupStyle style, PopupButtons buttons);

		static inline PopupSelection Show(::std::string title, ::std::string message, PopupStyle style)
		{
			return Show(title, message, style, PopupButtons::Ok);
		}

		static inline PopupSelection Show(::std::string title, ::std::string message, PopupButtons buttons)
		{
			return Show(title, message, PopupStyle::Info, buttons);
		}

		static inline PopupSelection Show(::std::string title, ::std::string message)
		{
			return Show(title, message, PopupStyle::Info, PopupButtons::Ok);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_POPUP_HPP */
