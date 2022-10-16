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

namespace np::gfx
{
	class RenderableLightObject : public RenderableObject
	{
	private:
		virtual void RenderToFrame(Frame& frame, Pipeline& pipline) override {} //lights do not need this

	public:
		virtual void RenderToFrame(Frame& frame, Pipeline& pipline, RenderableObject& object) = 0;
	};
} // namespace np::gfx

#endif /* NP_ENGINE_RPI_RENDERABLE_LIGHT_OBJECT_HPP */