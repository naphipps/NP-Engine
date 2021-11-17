//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP
#define NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "../../RPI/Renderer.hpp"

//TODO: add summary comments

namespace np::graphics::rhi
{
	class OpenGLRenderer : public Renderer
	{
	public:
		
		Renderer::RhiType GetRhiType() const override;
		bl IsEnabled() const override;
		str GetName() const override { return "OpenGL"; }
	};
}

#endif /* NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP */