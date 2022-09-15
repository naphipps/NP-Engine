//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_KEY_CODE_HPP
#define NP_ENGINE_KEY_CODE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "InputState.hpp"

namespace np::nput
{
	enum class KeyCode : ui32
	{
		Unkown = 0,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		Space,
		LeftShift,
		RightShift,
		LeftCtrl,
		RightCtrl,
		LeftAlt,
		RightAlt,
		Tab,
		Tilde,
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,
		Zero,
		Minus,
		Add,
		Multiply,
		Equal,
		Backspace,
		LeftSquareBracket,
		RightSquareBracket,
		Semicolon,
		Apostrophe,
		Comma,
		Period,
		Backslash,
		ForwardSlash,
		Enter,
		ArrowUp,
		ArrowLeft,
		ArrowRight,
		ArrowDown,
		Delete,
		Insert,
		Home,
		End,
		PageUp,
		PageDown,
		GraveAccent,
		Escape,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		F13,
		F14,
		F15,
		F16,
		F17,
		F18,
		F19,
		F20,
		F21,
		F22,
		F23,
		F24,
		F25,
		LeftSuper,
		RightSuper,
		CapsLock,
		ScrollLock,
		NumLock,
		PrintScreen,
		Pause,
		Fn,
		Menu,
		Max
	};

	using KeyCodeState = InputState<KeyCode>;

	static str GetKeyCodeName(KeyCode code)
	{
		str name;

		switch (code)
		{
		case KeyCode::A:
			name = "A";
			break;

		case KeyCode::B:
			name = "B";
			break;

		case KeyCode::C:
			name = "C";
			break;

		case KeyCode::D:
			name = "D";
			break;

		case KeyCode::E:
			name = "E";
			break;

		case KeyCode::F:
			name = "F";
			break;

		case KeyCode::G:
			name = "G";
			break;

		case KeyCode::H:
			name = "H";
			break;

		case KeyCode::I:
			name = "I";
			break;

		case KeyCode::J:
			name = "J";
			break;

		case KeyCode::K:
			name = "K";
			break;

		case KeyCode::L:
			name = "L";
			break;

		case KeyCode::M:
			name = "M";
			break;

		case KeyCode::N:
			name = "N";
			break;

		case KeyCode::O:
			name = "O";
			break;

		case KeyCode::P:
			name = "P";
			break;

		case KeyCode::Q:
			name = "Q";
			break;

		case KeyCode::R:
			name = "R";
			break;

		case KeyCode::S:
			name = "S";
			break;

		case KeyCode::T:
			name = "T";
			break;

		case KeyCode::U:
			name = "U";
			break;

		case KeyCode::V:
			name = "V";
			break;

		case KeyCode::W:
			name = "W";
			break;

		case KeyCode::X:
			name = "X";
			break;

		case KeyCode::Y:
			name = "Y";
			break;

		case KeyCode::Z:
			name = "Z";
			break;

		case KeyCode::Space:
			name = "Space";
			break;

		case KeyCode::LeftShift:
			name = "LeftShift";
			break;

		case KeyCode::RightShift:
			name = "RightShift";
			break;

		case KeyCode::LeftCtrl:
			name = "LeftCtrl";
			break;

		case KeyCode::RightCtrl:
			name = "RightCtrl";
			break;

		case KeyCode::LeftAlt:
			name = "LeftAlt";
			break;

		case KeyCode::RightAlt:
			name = "RightAlt";
			break;

		case KeyCode::Tab:
			name = "Tab";
			break;

		case KeyCode::Tilde:
			name = "Tilde";
			break;

		case KeyCode::One:
			name = "One";
			break;

		case KeyCode::Two:
			name = "Two";
			break;

		case KeyCode::Three:
			name = "Three";
			break;

		case KeyCode::Four:
			name = "Four";
			break;

		case KeyCode::Five:
			name = "Five";
			break;

		case KeyCode::Six:
			name = "Six";
			break;

		case KeyCode::Seven:
			name = "Seven";
			break;

		case KeyCode::Eight:
			name = "Eight";
			break;

		case KeyCode::Nine:
			name = "Nine";
			break;

		case KeyCode::Zero:
			name = "Zero";
			break;

		case KeyCode::Minus:
			name = "Minus";
			break;

		case KeyCode::Add:
			name = "Add";
			break;

		case KeyCode::Multiply:
			name = "Multiply";
			break;

		case KeyCode::Equal:
			name = "Equal";
			break;

		case KeyCode::Backspace:
			name = "Backspace";
			break;

		case KeyCode::LeftSquareBracket:
			name = "LeftSquareBracket";
			break;

		case KeyCode::RightSquareBracket:
			name = "RightSquareBracket";
			break;

		case KeyCode::Semicolon:
			name = "Semicolon";
			break;

		case KeyCode::Apostrophe:
			name = "Apostrophe";
			break;

		case KeyCode::Comma:
			name = "Comma";
			break;

		case KeyCode::Period:
			name = "Period";
			break;

		case KeyCode::Backslash:
			name = "Backslash";
			break;

		case KeyCode::ForwardSlash:
			name = "ForwardSlash";
			break;

		case KeyCode::Enter:
			name = "Enter";
			break;

		case KeyCode::ArrowUp:
			name = "ArrowUp";
			break;

		case KeyCode::ArrowLeft:
			name = "ArrowLeft";
			break;

		case KeyCode::ArrowRight:
			name = "ArrowRight";
			break;

		case KeyCode::ArrowDown:
			name = "ArrowDown";
			break;

		case KeyCode::Delete:
			name = "Delete";
			break;

		case KeyCode::Insert:
			name = "Insert";
			break;

		case KeyCode::Home:
			name = "Home";
			break;

		case KeyCode::End:
			name = "End";
			break;

		case KeyCode::PageUp:
			name = "PageUp";
			break;

		case KeyCode::PageDown:
			name = "PageDown";
			break;

		case KeyCode::GraveAccent:
			name = "GraveAccent";
			break;

		case KeyCode::Escape:
			name = "Escape";
			break;

		case KeyCode::F1:
			name = "F1";
			break;

		case KeyCode::F2:
			name = "F2";
			break;

		case KeyCode::F3:
			name = "F3";
			break;

		case KeyCode::F4:
			name = "F4";
			break;

		case KeyCode::F5:
			name = "F5";
			break;

		case KeyCode::F6:
			name = "F6";
			break;

		case KeyCode::F7:
			name = "F7";
			break;

		case KeyCode::F8:
			name = "F8";
			break;

		case KeyCode::F9:
			name = "F9";
			break;

		case KeyCode::F10:
			name = "F10";
			break;

		case KeyCode::F11:
			name = "F11";
			break;

		case KeyCode::F12:
			name = "F12";
			break;

		case KeyCode::F13:
			name = "F13";
			break;

		case KeyCode::F14:
			name = "F14";
			break;

		case KeyCode::F15:
			name = "F15";
			break;

		case KeyCode::F16:
			name = "F16";
			break;

		case KeyCode::F17:
			name = "F17";
			break;

		case KeyCode::F18:
			name = "F18";
			break;

		case KeyCode::F19:
			name = "F19";
			break;

		case KeyCode::F20:
			name = "F20";
			break;

		case KeyCode::F21:
			name = "F21";
			break;

		case KeyCode::F22:
			name = "F22";
			break;

		case KeyCode::F23:
			name = "F23";
			break;

		case KeyCode::F24:
			name = "F24";
			break;

		case KeyCode::F25:
			name = "F25";
			break;

		case KeyCode::LeftSuper:
			name = "LeftSuper";
			break;

		case KeyCode::RightSuper:
			name = "RightSuper";
			break;

		case KeyCode::CapsLock:
			name = "CapsLock";
			break;

		case KeyCode::ScrollLock:
			name = "ScrollLock";
			break;

		case KeyCode::NumLock:
			name = "NumLock";
			break;

		case KeyCode::PrintScreen:
			name = "PrintScreen";
			break;

		case KeyCode::Pause:
			name = "Pause";
			break;

		case KeyCode::Fn:
			name = "Fn";
			break;

		case KeyCode::Menu:
			name = "Menu";
			break;

		case KeyCode::Max:
		case KeyCode::Unkown:
		default:
			name = "Unkown";
			break;
		}

		return name;
	}
} // namespace np::nput

#endif /* NP_ENGINE_KEY_CODE_HPP */
