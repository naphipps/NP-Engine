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

namespace np::gfx
{
	struct RenderableMetaValues
	{
		ui8 _padding[128];
		//TODO: I think our renderables need to define a struct output-like object
		//	^ and then the implementation/detail can just grab the largest for the pipeline's layout (max of 128? what is vulkan's max??)
		//	^ - we will have to make sure that our shaders align

	};

	struct RenderableMetaValues_old
	{
		::glm::mat4 transform;

		union {
			struct
			{
				::glm::mat4 normal; //TODO: ?
			} object;

			struct
			{
				::glm::vec4 color;
				flt radius;
			} light;
		};
	};

	NP_ENGINE_STATIC_ASSERT(sizeof(RenderableMetaValues) <= 128, "RenderableMetaValues should never exceed 128 bytes.");
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_META_VALUES_HPP */