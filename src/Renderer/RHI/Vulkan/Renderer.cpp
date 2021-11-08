//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Renderer/RHI/Vulkan/Renderer.hpp"

//TODO: add summary comments

namespace np::renderer::rhi
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