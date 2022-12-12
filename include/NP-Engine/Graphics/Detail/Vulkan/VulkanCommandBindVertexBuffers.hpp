//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_BIND_VERTEX_BUFFERS_HPP
#define NP_ENGINE_VULKAN_COMMAND_BIND_VERTEX_BUFFERS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::gfx::__detail
{
	class VulkanCommandBindVertexBuffers : public VulkanCommand
	{
	public:
		ui32 FirstBinding;
		ui32 BindingCount;
		VkBuffer* Buffers;
		VkDeviceSize* Offsets;

		VulkanCommandBindVertexBuffers(ui32 first_binding, ui32 binding_count, VkBuffer* buffers, VkDeviceSize* offsets):
			FirstBinding(first_binding),
			BindingCount(binding_count),
			Buffers(buffers),
			Offsets(offsets)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindVertexBuffers(command_buffer, FirstBinding, BindingCount, Buffers, Offsets);
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_COMMAND_BIND_VERTEX_BUFFERS_HPP */