//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_COMMAND_BUFFER_HPP
#define NP_ENGINE_GPU_VULKAN_COMMAND_BUFFER_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/CommandBuffer.hpp"

namespace np::gpu::__detail
{
	class VulkanCommandBufferUsage : public CommandBufferUsage
	{
	public:
		VulkanCommandBufferUsage(ui32 value): CommandBufferUsage(value) {}

		VkCommandBufferUsageFlags GetVkCommandBufferUsageFlags() const
		{
			/*
				VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT = 0x00000002,
				VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT = 0x00000004,
				VK_COMMAND_BUFFER_USAGE_FLAG_BITS_MAX_ENUM = 0x7FFFFFFF
			*/

			VkCommandBufferUsageFlags flags = 0;

			if (Contains(SingleUse))
				flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

			return flags;
		}

		VkCommandBufferResetFlags GetVkCommandBufferResetFlags() const
		{
			/*
				VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT = 0x00000001,
			*/

			VkCommandBufferResetFlags flags = 0;
			return flags;
		}
	};

	class VulkanCommandBuffer : public CommandBuffer
	{
	private:
		mem::sptr<srvc::Services> _services;
		VkCommandBuffer _command_buffer;
		con::vector<mem::sptr<VulkanCommandBuffer>> _depenedencies;

	public:
		VulkanCommandBuffer(mem::sptr<srvc::Services> services, VkCommandBuffer command_buffer):
			_services(services),
			_command_buffer(command_buffer),
			_depenedencies{}
		{}

		~VulkanCommandBuffer() = default;

		operator VkCommandBuffer() const
		{
			return _command_buffer;
		}

		virtual DetailType GetDetailType() const
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _services;
		}

		virtual bl DependOn(mem::sptr<CommandBuffer> other) override
		{
			return false; //TODO: is this even a thing?
		}

		virtual con::vector<mem::sptr<CommandBuffer>> GetDependencies() const override
		{
			return {_depenedencies.begin(), _depenedencies.end()};
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_COMMAND_BUFFER_HPP */
