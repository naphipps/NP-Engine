//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RHI/Vulkan/Renderer.hpp"

//TODO: add summary comments

namespace np::graphics::rhi
{
	bl VulkanRenderer::Init()
	{
		return false;
	}

	rpi::Renderer::Type VulkanRenderer::GetType() const
	{
		return rpi::Renderer::Type::Vulkan;
	}
}