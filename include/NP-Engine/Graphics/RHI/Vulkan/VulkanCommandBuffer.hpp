//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_BUFFER_HPP
#define NP_ENGINE_VULKAN_COMMAND_BUFFER_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::graphics::rhi
{
	class VulkanCommandBuffer
	{
	private:
		VkCommandBuffer _command_buffer;

	public:
		VulkanCommandBuffer(VkCommandBuffer command_buffer): _command_buffer(command_buffer) {}

		~VulkanCommandBuffer() = default; // TODO: do we need this??

		operator VkCommandBuffer() const
		{
			return _command_buffer;
		}

		VkResult Begin(VkCommandBufferBeginInfo& command_buffer_begin_info)
		{
			return vkBeginCommandBuffer(_command_buffer, &command_buffer_begin_info);
		}

		VkResult End()
		{
			return vkEndCommandBuffer(_command_buffer);
		}

		void Add(VulkanCommand& command)
		{
			command.ApplyTo(_command_buffer);
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_BUFFER_HPP */
