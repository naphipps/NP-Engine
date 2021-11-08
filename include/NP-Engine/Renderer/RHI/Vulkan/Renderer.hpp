//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_RENDERER_VULKAN_RENDERER_HPP
#define NP_ENGINE_RENDERER_VULKAN_RENDERER_HPP

#include "NP-Engine/Renderer/RPI/Renderer.hpp"

//TODO: add summary comments

namespace np::renderer::rhi
{
	class VulkanRenderer : public rpi::Renderer
	{
	public:
		bl Init() override;
		rpi::Renderer::Type GetType() const override;
	};
}

#endif /* NP_ENGINE_RENDERER_VULKAN_RENDERER_HPP */