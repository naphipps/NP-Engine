//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_OBJECT_META_VALUES_HPP
#define NP_ENGINE_RPI_OBJECT_META_VALUES_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Foundation/Foundation.hpp"

#include "NP-Engine/Vendor/GlmInclude.hpp"

namespace np::graphics
{
	struct ObjectMetaValues // TODO: we only use this for Renderable objects, so maybe rename to RenderableMetaValues?
	{
		union {
			struct
			{
				::glm::mat4 Model;
				::glm::mat4 Normal;
			};

			struct
			{
				::glm::vec4 Color;
				::glm::vec3 Position;
				flt Radius;
			};
		};
	};

	NP_ENGINE_STATIC_ASSERT(sizeof(ObjectMetaValues) <= 128, "ObjectMetaValues should never exceed 128 bytes.");
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_OBJECT_META_VALUES_HPP */