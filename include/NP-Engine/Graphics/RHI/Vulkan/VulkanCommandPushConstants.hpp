//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_PUSH_CONSTANTS_HPP
#define NP_ENGINE_VULKAN_COMMAND_PUSH_CONSTANTS_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::gfx::rhi
{
	class VulkanCommandPushConstants : public VulkanCommand
	{
	public:
		VkPipelineLayout PipelineLayout;
		VkShaderStageFlags ShaderStageFlags;
		ui32 Offset;
		ui32 Size;
		void* Values;

		VulkanCommandPushConstants(VkPipelineLayout pipeline_layout, VkShaderStageFlags shader_stage_flags, ui32 offset,
								   ui32 size, void* values):
			PipelineLayout(pipeline_layout),
			ShaderStageFlags(shader_stage_flags),
			Offset(offset),
			Size(size),
			Values(values)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdPushConstants(command_buffer, PipelineLayout, ShaderStageFlags, Offset, Size, Values);
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_PUSH_CONSTANTS_HPP */