//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_POPUP_HPP
#define NP_ENGINE_POPUP_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/String/String.hpp"

// TODO: add summary comments

namespace np::app
{
	class Popup
	{
	public:
		enum class Select
		{
			OK,
			Cancel,
			Yes,
			No,
			Quit,
			None,
			Error
		};

		enum class Style
		{
			Info,
			Warning,
			Error,
			Question
		};

		enum class Buttons
		{
			OK,
			OKCancel,
			YesNo,
			YesNoCancel,
			Quit
		};

		constexpr static Select DefaultSelect = Select::OK;
		constexpr static Style DefaultStyle = Style::Info;
		constexpr static Buttons DefaultButtons = Buttons::OK;

		static Select Show(str title, str message, Style style, Buttons buttons);

		static inline Select Show(str title, str message, Style style)
		{
			return Show(title, message, style, DefaultButtons);
		}

		static inline Select Show(str title, str message, Buttons buttons)
		{
			return Show(title, message, DefaultStyle, buttons);
		}

		static inline Select Show(str title, str message)
		{
			return Show(title, message, DefaultStyle, DefaultButtons);
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_POPUP_HPP */
