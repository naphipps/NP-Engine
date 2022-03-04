//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_END_RENDER_PASS_HPP
#define NP_ENGINE_VULKAN_COMMAND_END_RENDER_PASS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::graphics::rhi
{
	class VulkanCommandEndRenderPass : public VulkanCommand
	{
	public:
		VulkanCommandEndRenderPass() {}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdEndRenderPass(command_buffer);
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_END_RENDER_PASS_HPP */