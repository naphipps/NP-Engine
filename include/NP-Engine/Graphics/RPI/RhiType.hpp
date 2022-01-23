//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/22/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RHI_TYPE_HPP
#define NP_ENGINE_RHI_TYPE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::graphics
{
	enum class RhiType : ui32
	{
		None,
		Vulkan,
		OpenGL,
		DirectX,
		Metal
	};

	namespace __detail
	{
		extern atm<RhiType> RegisteredRhiType;
	} // namespace __detail
} // namespace np::graphics

#endif /* NP_ENGINE_RHI_TYPE_HPP */