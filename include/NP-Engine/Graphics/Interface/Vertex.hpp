//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_VERTEX_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_VERTEX_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::gfx
{
	struct Vertex
	{
		::glm::vec3 Position;
		::glm::vec3 Color; // TODO: added alpha channel support
		::glm::vec2 TextureCoordinate;

		bl operator==(const Vertex& other) const
		{
			return Position == other.Position && Color == other.Color && TextureCoordinate == other.TextureCoordinate;
		}
	};
} // namespace np::gfx

namespace std
{
	template <>
	struct hash<::np::gfx::Vertex>
	{
		siz operator()(const ::np::gfx::Vertex& vertex) const noexcept
		{
			return (hash<::glm::vec3>()(vertex.Position) ^ (hash<::glm::vec3>()(vertex.Color) << 1) >> 1) ^
				(hash<::glm::vec2>()(vertex.TextureCoordinate) << 1);
		}
	};
} // namespace std

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_VERTEX_HPP */