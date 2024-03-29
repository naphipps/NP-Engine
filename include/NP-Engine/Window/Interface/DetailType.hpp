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
	enum class DetailType : ui32
	{
		None,
		Glfw,
		Sdl
	};
} // namespace np::win

#endif /* NP_ENGINE_WINDOW_DETAIL_TYPE_HPP */