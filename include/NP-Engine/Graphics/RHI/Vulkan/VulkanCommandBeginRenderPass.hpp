//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_BEGIN_RENDER_PASS_HPP
#define NP_ENGINE_VULKAN_COMMAND_BEGIN_RENDER_PASS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::graphics::rhi
{
	class VulkanCommandBeginRenderPass : public VulkanCommand
	{
	public:
		VkRenderPassBeginInfo RenderPassBeginInfo;
		VkSubpassContents SubpassContents;

		VulkanCommandBeginRenderPass(VkRenderPassBeginInfo render_pass_begin_info, VkSubpassContents subpass_contents):
			RenderPassBeginInfo(render_pass_begin_info),
			SubpassContents(subpass_contents)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBeginRenderPass(command_buffer, &RenderPassBeginInfo, SubpassContents);
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_BEGIN_RENDER_PASS_HPP */