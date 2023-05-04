//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_META_VALUES_HPP
#define NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_META_VALUES_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::gpu
{
	struct RenderableMetaValues //TODO: this is old - try new technique
	{
		union {
			struct
			{
				::glm::mat4 Model;
				::glm::mat4 Normal;
			} object;

			struct
			{
				::glm::vec4 Color;
				::glm::vec3 Position;
				flt Radius;
			} light;
		};
	};

	struct RenderableMetaValues_new2
	{
		//TODO: I think our renderables need to define a struct output-like object
		//	^ and then the implementation/detail can just grab the largest for the pipeline's layout (max of 128? what is vulkan's max??)
		//	^ - we will have to make sure that our shaders align

		::glm::mat4 transform;

		union {
			struct
			{
				::glm::mat4 normal; //TODO: ? I have TODO here without a comment, so I'm wondering why...
			} object;

			struct
			{
				::glm::vec4 color;
				flt radius;
			} light;
		};
	};

	NP_ENGINE_STATIC_ASSERT(sizeof(RenderableMetaValues) <= 128, "RenderableMetaValues should never exceed 128 bytes.");
} // namespace np::gpu

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_META_VALUES_HPP */