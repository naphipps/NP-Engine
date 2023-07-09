//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_KEY_CODE_HPP
#define NP_ENGINE_KEY_CODE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "InputState.hpp"

namespace np
{
	namespace nput
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
		using KeyCodeStates = InputStates<KeyCode, (siz)KeyCode::Max>;
		using KeyCallback = void (*)(void* caller, const nput::KeyCodeState& key_code_state);
	} // namespace nput

	static str to_str(nput::KeyCode code)
	{
		str name;

		switch (code)
		{
		case nput::KeyCode::A:
			name = "A";
			break;

		case nput::KeyCode::B:
			name = "B";
			break;

		case nput::KeyCode::C:
			name = "C";
			break;

		case nput::KeyCode::D:
			name = "D";
			break;

		case nput::KeyCode::E:
			name = "E";
			break;

		case nput::KeyCode::F:
			name = "F";
			break;

		case nput::KeyCode::G:
			name = "G";
			break;

		case nput::KeyCode::H:
			name = "H";
			break;

		case nput::KeyCode::I:
			name = "I";
			break;

		case nput::KeyCode::J:
			name = "J";
			break;

		case nput::KeyCode::K:
			name = "K";
			break;

		case nput::KeyCode::L:
			name = "L";
			break;

		case nput::KeyCode::M:
			name = "M";
			break;

		case nput::KeyCode::N:
			name = "N";
			break;

		case nput::KeyCode::O:
			name = "O";
			break;

		case nput::KeyCode::P:
			name = "P";
			break;

		case nput::KeyCode::Q:
			name = "Q";
			break;

		case nput::KeyCode::R:
			name = "R";
			break;

		case nput::KeyCode::S:
			name = "S";
			break;

		case nput::KeyCode::T:
			name = "T";
			break;

		case nput::KeyCode::U:
			name = "U";
			break;

		case nput::KeyCode::V:
			name = "V";
			break;

		case nput::KeyCode::W:
			name = "W";
			break;

		case nput::KeyCode::X:
			name = "X";
			break;

		case nput::KeyCode::Y:
			name = "Y";
			break;

		case nput::KeyCode::Z:
			name = "Z";
			break;

		case nput::KeyCode::Space:
			name = "Space";
			break;

		case nput::KeyCode::LeftShift:
			name = "LeftShift";
			break;

		case nput::KeyCode::RightShift:
			name = "RightShift";
			break;

		case nput::KeyCode::LeftCtrl:
			name = "LeftCtrl";
			break;

		case nput::KeyCode::RightCtrl:
			name = "RightCtrl";
			break;

		case nput::KeyCode::LeftAlt:
			name = "LeftAlt";
			break;

		case nput::KeyCode::RightAlt:
			name = "RightAlt";
			break;

		case nput::KeyCode::Tab:
			name = "Tab";
			break;

		case nput::KeyCode::Tilde:
			name = "Tilde";
			break;

		case nput::KeyCode::One:
			name = "One";
			break;

		case nput::KeyCode::Two:
			name = "Two";
			break;

		case nput::KeyCode::Three:
			name = "Three";
			break;

		case nput::KeyCode::Four:
			name = "Four";
			break;

		case nput::KeyCode::Five:
			name = "Five";
			break;

		case nput::KeyCode::Six:
			name = "Six";
			break;

		case nput::KeyCode::Seven:
			name = "Seven";
			break;

		case nput::KeyCode::Eight:
			name = "Eight";
			break;

		case nput::KeyCode::Nine:
			name = "Nine";
			break;

		case nput::KeyCode::Zero:
			name = "Zero";
			break;

		case nput::KeyCode::Minus:
			name = "Minus";
			break;

		case nput::KeyCode::Add:
			name = "Add";
			break;

		case nput::KeyCode::Multiply:
			name = "Multiply";
			break;

		case nput::KeyCode::Equal:
			name = "Equal";
			break;

		case nput::KeyCode::Backspace:
			name = "Backspace";
			break;

		case nput::KeyCode::LeftSquareBracket:
			name = "LeftSquareBracket";
			break;

		case nput::KeyCode::RightSquareBracket:
			name = "RightSquareBracket";
			break;

		case nput::KeyCode::Semicolon:
			name = "Semicolon";
			break;

		case nput::KeyCode::Apostrophe:
			name = "Apostrophe";
			break;

		case nput::KeyCode::Comma:
			name = "Comma";
			break;

		case nput::KeyCode::Period:
			name = "Period";
			break;

		case nput::KeyCode::Backslash:
			name = "Backslash";
			break;

		case nput::KeyCode::ForwardSlash:
			name = "ForwardSlash";
			break;

		case nput::KeyCode::Enter:
			name = "Enter";
			break;

		case nput::KeyCode::ArrowUp:
			name = "ArrowUp";
			break;

		case nput::KeyCode::ArrowLeft:
			name = "ArrowLeft";
			break;

		case nput::KeyCode::ArrowRight:
			name = "ArrowRight";
			break;

		case nput::KeyCode::ArrowDown:
			name = "ArrowDown";
			break;

		case nput::KeyCode::Delete:
			name = "Delete";
			break;

		case nput::KeyCode::Insert:
			name = "Insert";
			break;

		case nput::KeyCode::Home:
			name = "Home";
			break;

		case nput::KeyCode::End:
			name = "End";
			break;

		case nput::KeyCode::PageUp:
			name = "PageUp";
			break;

		case nput::KeyCode::PageDown:
			name = "PageDown";
			break;

		case nput::KeyCode::GraveAccent:
			name = "GraveAccent";
			break;

		case nput::KeyCode::Escape:
			name = "Escape";
			break;

		case nput::KeyCode::F1:
			name = "F1";
			break;

		case nput::KeyCode::F2:
			name = "F2";
			break;

		case nput::KeyCode::F3:
			name = "F3";
			break;

		case nput::KeyCode::F4:
			name = "F4";
			break;

		case nput::KeyCode::F5:
			name = "F5";
			break;

		case nput::KeyCode::F6:
			name = "F6";
			break;

		case nput::KeyCode::F7:
			name = "F7";
			break;

		case nput::KeyCode::F8:
			name = "F8";
			break;

		case nput::KeyCode::F9:
			name = "F9";
			break;

		case nput::KeyCode::F10:
			name = "F10";
			break;

		case nput::KeyCode::F11:
			name = "F11";
			break;

		case nput::KeyCode::F12:
			name = "F12";
			break;

		case nput::KeyCode::F13:
			name = "F13";
			break;

		case nput::KeyCode::F14:
			name = "F14";
			break;

		case nput::KeyCode::F15:
			name = "F15";
			break;

		case nput::KeyCode::F16:
			name = "F16";
			break;

		case nput::KeyCode::F17:
			name = "F17";
			break;

		case nput::KeyCode::F18:
			name = "F18";
			break;

		case nput::KeyCode::F19:
			name = "F19";
			break;

		case nput::KeyCode::F20:
			name = "F20";
			break;

		case nput::KeyCode::F21:
			name = "F21";
			break;

		case nput::KeyCode::F22:
			name = "F22";
			break;

		case nput::KeyCode::F23:
			name = "F23";
			break;

		case nput::KeyCode::F24:
			name = "F24";
			break;

		case nput::KeyCode::F25:
			name = "F25";
			break;

		case nput::KeyCode::LeftSuper:
			name = "LeftSuper";
			break;

		case nput::KeyCode::RightSuper:
			name = "RightSuper";
			break;

		case nput::KeyCode::CapsLock:
			name = "CapsLock";
			break;

		case nput::KeyCode::ScrollLock:
			name = "ScrollLock";
			break;

		case nput::KeyCode::NumLock:
			name = "NumLock";
			break;

		case nput::KeyCode::PrintScreen:
			name = "PrintScreen";
			break;

		case nput::KeyCode::Pause:
			name = "Pause";
			break;

		case nput::KeyCode::Fn:
			name = "Fn";
			break;

		case nput::KeyCode::Menu:
			name = "Menu";
			break;

		case nput::KeyCode::Max:
		case nput::KeyCode::Unkown:
		default:
			name = "Unkown";
			break;
		}

		return name;
	}
} // namespace np

#endif /* NP_ENGINE_KEY_CODE_HPP */
