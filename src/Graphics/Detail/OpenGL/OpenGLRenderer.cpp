//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLRenderer.hpp"

namespace np::gfx::rhi
{
	Renderer::RhiType OpenGLRenderer::GetRhiType() const
	{
		return Renderer::RhiType::OpenGL;
	}

	bl OpenGLRenderer::IsEnabled() const
	{
		return false;
	}
} // namespace np::gfx::rhi