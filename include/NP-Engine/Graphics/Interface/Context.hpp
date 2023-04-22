//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_CONTEXT_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_CONTEXT_HPP

#include "NP-Engine/Services/Services.hpp"

#include "GraphicsDetailType.hpp"

namespace np::gfx
{
	class Context
	{
		//TODO: I think our other contexts inherit from this?
	protected:

		Context() {}

	public:

		virtual GraphicsDetailType GetDetailType() const = 0;
	};
}

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_CONTEXT_HPP */