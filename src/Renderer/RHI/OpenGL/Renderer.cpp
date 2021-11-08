//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Renderer/RHI/OpenGL/Renderer.hpp"

//TODO: add summary comments

namespace np::renderer::rhi
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