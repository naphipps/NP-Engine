//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_VERTEX_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_VERTEX_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::gpu
{
	struct Vertex
	{
		//TODO: camelCase here

		::glm::vec3 Position;
		::glm::vec3 Color; // TODO: added alpha channel support
		::glm::vec2 TextureCoordinate;

		bl operator==(const Vertex& other) const
		{
			return Position == other.Position && Color == other.Color && TextureCoordinate == other.TextureCoordinate;
		}
	};
} // namespace np::gpu

namespace std
{
	template <>
	struct hash<::np::gpu::Vertex>
	{
		::np::siz operator()(const ::np::gpu::Vertex& vertex) const noexcept
		{
			//treat the vertex like a buffer and hash the whole thing
			return ::np::mat::HashFnv1aUi64(::np::mem::AddressOf(vertex), sizeof(::np::gpu::Vertex));
		}
	};
} // namespace std

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_VERTEX_HPP */