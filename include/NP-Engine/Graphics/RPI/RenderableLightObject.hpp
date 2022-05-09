//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/28/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RPI_RENDERABLE_LIGHT_OBJECT_HPP
#define NP_ENGINE_RPI_RENDERABLE_LIGHT_OBJECT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Frame.hpp"
#include "Pipeline.hpp"
#include "RenderableType.hpp"
#include "RenderableObject.hpp"

namespace np::graphics
{
	struct RenderableLightObject
	{
		virtual RenderableType GetType() const = 0;

		virtual void RenderToFrame(Frame& frame, Pipeline& pipline, RenderableObject& object) = 0;

		virtual void PrepareForPipeline(Pipeline& pipeline) = 0;

		virtual void DisposeForPipeline(Pipeline& pipeline) = 0;

		virtual bl IsOutOfDate() const = 0;

		virtual void SetOutOfDate(bl is_out_of_date = true) = 0;
	};
} // namespace np::graphics

#endif /* NP_ENGINE_RPI_RENDERABLE_LIGHT_OBJECT_HPP */