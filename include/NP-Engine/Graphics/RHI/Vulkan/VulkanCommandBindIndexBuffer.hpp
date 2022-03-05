//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_BIND_INDEX_BUFFER_HPP
#define NP_ENGINE_VULKAN_COMMAND_BIND_INDEX_BUFFER_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::graphics::rhi
{
	class VulkanCommandBindIndexBuffer : public VulkanCommand
	{
	public:
		VkBuffer Buffer;
		VkDeviceSize Offset;
		VkIndexType IndexType;

		VulkanCommandBindIndexBuffer(VkBuffer buffer, VkDeviceSize offset, VkIndexType index_type):
			Buffer(buffer),
			Offset(offset),
			IndexType(index_type)
		{}

		void ApplyTo(VkCommandBuffer command_buffer) override
		{
			vkCmdBindIndexBuffer(command_buffer, Buffer, Offset, IndexType);
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_BIND_INDEX_BUFFER_HPP */