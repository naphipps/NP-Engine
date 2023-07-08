//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_VERTEX_HPP
#define NP_ENGINE_GPU_INTERFACE_VERTEX_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::gpu
{
	struct Vertex
	{
		::glm::vec3 position{0.0f};
		::glm::vec3 color{1.0f}; // TODO: add alpha channel support
		::glm::vec2 uv{0.0f}; //texture coordinate
		//::glm::vec3 normal; //TODO: should our vertices have normals? I think this should be understood by vertex order (CCW / CW)

		bl operator==(const Vertex& other) const
		{
			return position == other.position && color == other.color && uv == other.uv;
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

#endif /* NP_ENGINE_GPU_INTERFACE_VERTEX_HPP */