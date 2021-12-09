//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_PIPELINE_HPP
#define NP_ENGINE_VULKAN_PIPELINE_HPP

#include "NP-Engine/Filesystem/Filesystem.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanDevice.hpp"
#include "VulkanShader.hpp"

namespace np::graphics::rhi
{
	class VulkanPipeline
	{
	private:
		VulkanDevice& _device;
		VulkanShader _vertex_shader;
		VulkanShader _fragment_shader;

	public:
		VulkanPipeline(VulkanDevice& device):
			_device(device),
			_vertex_shader(fs::append("Vulkan", fs::append("shaders", "vertex.glsl")), VulkanShader::Type::VERTEX),
			_fragment_shader(fs::append("Vulkan", fs::append("shaders", "fragment.glsl")), VulkanShader::Type::FRAGMENT)
		{}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_PIPELINE_HPP */