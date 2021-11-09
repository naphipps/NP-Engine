//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RHI/OpenGL/Renderer.hpp"

//TODO: add summary comments

namespace np::graphics::rhi
{
	bl OpenGLRenderer::Init()
	{
		return false;
	}

	rpi::Renderer::Type OpenGLRenderer::GetType() const
	{
		return rpi::Renderer::Type::OpenGL;
	}
}