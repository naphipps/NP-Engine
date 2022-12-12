//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_BUFFER_HPP
#define NP_ENGINE_VULKAN_COMMAND_BUFFER_HPP

#include <utility>

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommand.hpp"

namespace np::gfx::rhi
{
	class VulkanCommandBuffer
	{
	private:
		VkCommandBuffer _command_buffer;

	public:
		static VkCommandBufferBeginInfo CreateBeginInfo()
		{
			VkCommandBufferBeginInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			return info;
		}

		VulkanCommandBuffer(): VulkanCommandBuffer(nullptr) {}

		VulkanCommandBuffer(VkCommandBuffer command_buffer): _command_buffer(command_buffer) {}

		VulkanCommandBuffer(const VulkanCommandBuffer& other): _command_buffer(other._command_buffer) {}

		VulkanCommandBuffer(VulkanCommandBuffer&& other) noexcept: _command_buffer(::std::move(other._command_buffer))
		{
			other._command_buffer = nullptr;
		}

		VulkanCommandBuffer& operator=(const VulkanCommandBuffer& other)
		{
			_command_buffer = other._command_buffer;
			return *this;
		}

		VulkanCommandBuffer& operator=(VulkanCommandBuffer&& other) noexcept
		{
			_command_buffer = ::std::move(other._command_buffer);
			other._command_buffer = nullptr;
			return *this;
		}

		operator VkCommandBuffer() const
		{
			return _command_buffer;
		}

		bl IsValid() const
		{
			return _command_buffer != nullptr;
		}

		void Invalidate()
		{
			_command_buffer = nullptr;
		}

		VkResult Begin(VkCommandBufferBeginInfo& command_buffer_begin_info)
		{
			NP_ENGINE_ASSERT(IsValid(), "VulkanCommandBuffer must be valid before Begin is called.");
			return vkBeginCommandBuffer(_command_buffer, &command_buffer_begin_info);
		}

		VkResult End()
		{
			NP_ENGINE_ASSERT(IsValid(), "VulkanCommandBuffer must be valid before End is called.");
			return vkEndCommandBuffer(_command_buffer);
		}

		void Add(VulkanCommand& command)
		{
			NP_ENGINE_ASSERT(IsValid(), "VulkanCommandBuffer must be valid before Add is called.");
			command.ApplyTo(_command_buffer);
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_BUFFER_HPP */
