//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_VISIBLE_OBJECT_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_VISIBLE_OBJECT_HPP

#include "NP-Engine/Geometry/Geometry.hpp"

#include "RenderableObject.hpp"

namespace np::gfx
{
	struct VisibleObject
	{
		geom::FltAabb3D AABB;
		RenderableObject* Renderable = nullptr;
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_VISIBLE_OBJECT_HPP */