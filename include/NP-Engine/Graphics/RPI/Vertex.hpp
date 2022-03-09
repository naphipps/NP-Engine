//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_VERTEX_HPP
#define NP_ENGINE_RPI_VERTEX_HPP

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::graphics
{
	struct Vertex
	{
		::glm::vec3 Position;
		::glm::vec3 Color;
		::glm::vec2 TextureCoordinate;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_VERTEX_HPP */