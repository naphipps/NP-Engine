//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_BIND_PIPELINE_HPP
#define NP_ENGINE_VULKAN_COMMAND_BIND_PIPELINE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::gfx::__detail
{
	class VulkanCommandBindPipeline : public VulkanCommand
	{
	public:
		VkPipelineBindPoint PipelineBindPoint;
		VkPipeline Pipeline;

		VulkanCommandBindPipeline(VkPipelineBindPoint pipeline_bind_point, VkPipeline pipeline):
			PipelineBindPoint(pipeline_bind_point),
			Pipeline(pipeline)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindPipeline(command_buffer, PipelineBindPoint, Pipeline);
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_COMMAND_BIND_PIPELINE_HPP */