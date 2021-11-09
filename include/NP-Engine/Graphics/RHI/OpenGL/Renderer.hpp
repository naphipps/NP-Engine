//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP
#define NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP

#include "NP-Engine/Graphics/RPI/Renderer.hpp"

//TODO: add summary comments

namespace np::graphics::rhi
{
	class OpenGLRenderer : public rpi::Renderer
	{
	public:
		bl Init() override;
		rpi::Renderer::Type GetType() const override;
	};
}

#endif /* NP_ENGINE_GRAPHICS_OPENGL_RENDERER_HPP */