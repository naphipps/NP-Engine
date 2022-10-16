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
	using KeyCodeStates = InputStates<KeyCode, (siz)KeyCode::Max>;
	using KeyCallback = void (*)(void* caller, const nput::KeyCodeState& key_code_state);
} // namespace np::nput

static str to_str(::np::nput::KeyCode code)
{
	str name;

	switch (code)
	{
	case ::np::nput::KeyCode::A:
		name = "A";
		break;

	case ::np::nput::KeyCode::B:
		name = "B";
		break;

	case ::np::nput::KeyCode::C:
		name = "C";
		break;

	case ::np::nput::KeyCode::D:
		name = "D";
		break;

	case ::np::nput::KeyCode::E:
		name = "E";
		break;

	case ::np::nput::KeyCode::F:
		name = "F";
		break;

	case ::np::nput::KeyCode::G:
		name = "G";
		break;

	case ::np::nput::KeyCode::H:
		name = "H";
		break;

	case ::np::nput::KeyCode::I:
		name = "I";
		break;

	case ::np::nput::KeyCode::J:
		name = "J";
		break;

	case ::np::nput::KeyCode::K:
		name = "K";
		break;

	case ::np::nput::KeyCode::L:
		name = "L";
		break;

	case ::np::nput::KeyCode::M:
		name = "M";
		break;

	case ::np::nput::KeyCode::N:
		name = "N";
		break;

	case ::np::nput::KeyCode::O:
		name = "O";
		break;

	case ::np::nput::KeyCode::P:
		name = "P";
		break;

	case ::np::nput::KeyCode::Q:
		name = "Q";
		break;

	case ::np::nput::KeyCode::R:
		name = "R";
		break;

	case ::np::nput::KeyCode::S:
		name = "S";
		break;

	case ::np::nput::KeyCode::T:
		name = "T";
		break;

	case ::np::nput::KeyCode::U:
		name = "U";
		break;

	case ::np::nput::KeyCode::V:
		name = "V";
		break;

	case ::np::nput::KeyCode::W:
		name = "W";
		break;

	case ::np::nput::KeyCode::X:
		name = "X";
		break;

	case ::np::nput::KeyCode::Y:
		name = "Y";
		break;

	case ::np::nput::KeyCode::Z:
		name = "Z";
		break;

	case ::np::nput::KeyCode::Space:
		name = "Space";
		break;

	case ::np::nput::KeyCode::LeftShift:
		name = "LeftShift";
		break;

	case ::np::nput::KeyCode::RightShift:
		name = "RightShift";
		break;

	case ::np::nput::KeyCode::LeftCtrl:
		name = "LeftCtrl";
		break;

	case ::np::nput::KeyCode::RightCtrl:
		name = "RightCtrl";
		break;

	case ::np::nput::KeyCode::LeftAlt:
		name = "LeftAlt";
		break;

	case ::np::nput::KeyCode::RightAlt:
		name = "RightAlt";
		break;

	case ::np::nput::KeyCode::Tab:
		name = "Tab";
		break;

	case ::np::nput::KeyCode::Tilde:
		name = "Tilde";
		break;

	case ::np::nput::KeyCode::One:
		name = "One";
		break;

	case ::np::nput::KeyCode::Two:
		name = "Two";
		break;

	case ::np::nput::KeyCode::Three:
		name = "Three";
		break;

	case ::np::nput::KeyCode::Four:
		name = "Four";
		break;

	case ::np::nput::KeyCode::Five:
		name = "Five";
		break;

	case ::np::nput::KeyCode::Six:
		name = "Six";
		break;

	case ::np::nput::KeyCode::Seven:
		name = "Seven";
		break;

	case ::np::nput::KeyCode::Eight:
		name = "Eight";
		break;

	case ::np::nput::KeyCode::Nine:
		name = "Nine";
		break;

	case ::np::nput::KeyCode::Zero:
		name = "Zero";
		break;

	case ::np::nput::KeyCode::Minus:
		name = "Minus";
		break;

	case ::np::nput::KeyCode::Add:
		name = "Add";
		break;

	case ::np::nput::KeyCode::Multiply:
		name = "Multiply";
		break;

	case ::np::nput::KeyCode::Equal:
		name = "Equal";
		break;

	case ::np::nput::KeyCode::Backspace:
		name = "Backspace";
		break;

	case ::np::nput::KeyCode::LeftSquareBracket:
		name = "LeftSquareBracket";
		break;

	case ::np::nput::KeyCode::RightSquareBracket:
		name = "RightSquareBracket";
		break;

	case ::np::nput::KeyCode::Semicolon:
		name = "Semicolon";
		break;

	case ::np::nput::KeyCode::Apostrophe:
		name = "Apostrophe";
		break;

	case ::np::nput::KeyCode::Comma:
		name = "Comma";
		break;

	case ::np::nput::KeyCode::Period:
		name = "Period";
		break;

	case ::np::nput::KeyCode::Backslash:
		name = "Backslash";
		break;

	case ::np::nput::KeyCode::ForwardSlash:
		name = "ForwardSlash";
		break;

	case ::np::nput::KeyCode::Enter:
		name = "Enter";
		break;

	case ::np::nput::KeyCode::ArrowUp:
		name = "ArrowUp";
		break;

	case ::np::nput::KeyCode::ArrowLeft:
		name = "ArrowLeft";
		break;

	case ::np::nput::KeyCode::ArrowRight:
		name = "ArrowRight";
		break;

	case ::np::nput::KeyCode::ArrowDown:
		name = "ArrowDown";
		break;

	case ::np::nput::KeyCode::Delete:
		name = "Delete";
		break;

	case ::np::nput::KeyCode::Insert:
		name = "Insert";
		break;

	case ::np::nput::KeyCode::Home:
		name = "Home";
		break;

	case ::np::nput::KeyCode::End:
		name = "End";
		break;

	case ::np::nput::KeyCode::PageUp:
		name = "PageUp";
		break;

	case ::np::nput::KeyCode::PageDown:
		name = "PageDown";
		break;

	case ::np::nput::KeyCode::GraveAccent:
		name = "GraveAccent";
		break;

	case ::np::nput::KeyCode::Escape:
		name = "Escape";
		break;

	case ::np::nput::KeyCode::F1:
		name = "F1";
		break;

	case ::np::nput::KeyCode::F2:
		name = "F2";
		break;

	case ::np::nput::KeyCode::F3:
		name = "F3";
		break;

	case ::np::nput::KeyCode::F4:
		name = "F4";
		break;

	case ::np::nput::KeyCode::F5:
		name = "F5";
		break;

	case ::np::nput::KeyCode::F6:
		name = "F6";
		break;

	case ::np::nput::KeyCode::F7:
		name = "F7";
		break;

	case ::np::nput::KeyCode::F8:
		name = "F8";
		break;

	case ::np::nput::KeyCode::F9:
		name = "F9";
		break;

	case ::np::nput::KeyCode::F10:
		name = "F10";
		break;

	case ::np::nput::KeyCode::F11:
		name = "F11";
		break;

	case ::np::nput::KeyCode::F12:
		name = "F12";
		break;

	case ::np::nput::KeyCode::F13:
		name = "F13";
		break;

	case ::np::nput::KeyCode::F14:
		name = "F14";
		break;

	case ::np::nput::KeyCode::F15:
		name = "F15";
		break;

	case ::np::nput::KeyCode::F16:
		name = "F16";
		break;

	case ::np::nput::KeyCode::F17:
		name = "F17";
		break;

	case ::np::nput::KeyCode::F18:
		name = "F18";
		break;

	case ::np::nput::KeyCode::F19:
		name = "F19";
		break;

	case ::np::nput::KeyCode::F20:
		name = "F20";
		break;

	case ::np::nput::KeyCode::F21:
		name = "F21";
		break;

	case ::np::nput::KeyCode::F22:
		name = "F22";
		break;

	case ::np::nput::KeyCode::F23:
		name = "F23";
		break;

	case ::np::nput::KeyCode::F24:
		name = "F24";
		break;

	case ::np::nput::KeyCode::F25:
		name = "F25";
		break;

	case ::np::nput::KeyCode::LeftSuper:
		name = "LeftSuper";
		break;

	case ::np::nput::KeyCode::RightSuper:
		name = "RightSuper";
		break;

	case ::np::nput::KeyCode::CapsLock:
		name = "CapsLock";
		break;

	case ::np::nput::KeyCode::ScrollLock:
		name = "ScrollLock";
		break;

	case ::np::nput::KeyCode::NumLock:
		name = "NumLock";
		break;

	case ::np::nput::KeyCode::PrintScreen:
		name = "PrintScreen";
		break;

	case ::np::nput::KeyCode::Pause:
		name = "Pause";
		break;

	case ::np::nput::KeyCode::Fn:
		name = "Fn";
		break;

	case ::np::nput::KeyCode::Menu:
		name = "Menu";
		break;

	case ::np::nput::KeyCode::Max:
	case ::np::nput::KeyCode::Unkown:
	default:
		name = "Unkown";
		break;
	}

	return name;
}

#endif /* NP_ENGINE_KEY_CODE_HPP */
