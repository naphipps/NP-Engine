//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RENDERABLE_META_VALUES_HPP
#define NP_ENGINE_GPU_INTERFACE_RENDERABLE_META_VALUES_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Geometry/Geometry.hpp"

namespace np::gpu
{
	struct RenderableMetaValues
	{
		// TODO: I think our renderables need to define a struct output-like object
		//	^ and then the implementation/detail can just grab the largest for the pipeline's layout (max of 128? what is
		//vulkan's max??) 	^ - we will have to make sure that our shaders align

		geom::Transform transform;
		ui32 resourceType; // TODO: ResourceType to ui32 so our shaders can respond properly

		constexpr static siz PADDING_SIZE = 128 - sizeof(geom::Transform) - sizeof(ui32);

		union {
			ui8 padding[PADDING_SIZE];

			struct
			{
				// TODO: I feel like we can get the light's color via their vertex
				::glm::vec4 color;
				flt radius;
			} light;
		};
	};

	NP_ENGINE_STATIC_ASSERT(sizeof(RenderableMetaValues) <= 128, "RenderableMetaValues should never exceed 128 bytes.");
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RENDERABLE_META_VALUES_HPP */