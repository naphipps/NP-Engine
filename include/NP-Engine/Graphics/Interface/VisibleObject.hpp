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
		geom::FltAabb3D aabb;
		glm::mat4 transform;
		
		//TODO: add data for the resource

		//AnimationData animationData;

		//TODO: should we add a transform for the Model/Image/Light? some way to update RenderableMetaValues
	};
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_VISIBLE_OBJECT_HPP */