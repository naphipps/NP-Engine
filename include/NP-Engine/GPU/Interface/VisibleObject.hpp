//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_VISIBLE_OBJECT_HPP
#define NP_ENGINE_GPU_INTERFACE_VISIBLE_OBJECT_HPP

#include "NP-Engine/Geometry/Geometry.hpp"

#include "RenderableObject.hpp"

namespace np::gpu
{
	struct VisibleObject
	{
		geom::FltAabb3D aabb;

		// TODO: add data for the resource

		// AnimationData animationData;

		// TODO: should we add a transform for the Model/Image/Light? some way to update RenderableMetaValues
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_VISIBLE_OBJECT_HPP */