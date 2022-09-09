//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_VISIBLE_OBJECT_HPP
#define NP_ENGINE_RPI_VISIBLE_OBJECT_HPP

#include "NP-Engine/Geometry/Geometry.hpp"

namespace np::gfx
{
	struct VisibleObject
	{
		geom::DblSphere BoundingSphere;
	};
}

#endif /* NP_ENGINE_RPI_VISIBLE_OBJECT_HPP */