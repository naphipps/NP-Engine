//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 10/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_WINDOW_DETAIL_TYPE_HPP
#define NP_ENGINE_WINDOW_DETAIL_TYPE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::win
{
	enum class WindowDetailType : ui32
	{
		None,
		Glfw,
		Sdl
	};

	namespace __detail
	{
		extern atm<WindowDetailType> RegisteredWindowDetailType;
	}

	static WindowDetailType GetRegisteredWindowDetailType()
	{
		return __detail::RegisteredWindowDetailType.load(mo_acquire);
	}
}

#endif /* NP_ENGINE_WINDOW_DETAIL_TYPE_HPP */