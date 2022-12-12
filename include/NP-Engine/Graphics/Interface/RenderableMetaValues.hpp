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

	NP_ENGINE_STATIC_ASSERT(sizeof(RenderableMetaValues) <= 128, "RenderableMetaValues should never exceed 128 bytes.");
} // namespace np::gfx

#endif /* NP_ENGINE_GRAPHICS_INTERFACE_RENDERABLE_META_VALUES_HPP */