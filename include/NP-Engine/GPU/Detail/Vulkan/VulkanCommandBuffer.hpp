//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_BUFFER_HPP
#define NP_ENGINE_VULKAN_COMMAND_BUFFER_HPP

#include <utility>

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommands.hpp"

namespace np::gpu::__detail
{
	class VulkanCommandBuffer : public CommandBuffer
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


		bl IsValid() const override
		{
			return _command_buffer != nullptr;
		}

		void Invalidate() override
		{
			_command_buffer = nullptr;
		}

		void Add(Command& command) override
		{
			NP_ENGINE_ASSERT(IsValid() && command.GetDetailType() == GraphicsDetailType::Vulkan,
				"VulkanCommandBuffer must be valid and given command must be a vulkan command before Add is called.");
			((VulkanCommand&)command).ApplyTo(_command_buffer);
		}

		VkResult Begin(VkCommandBufferBeginInfo& command_buffer_begin_info) //TODO: should this be in Device??
		{
			NP_ENGINE_ASSERT(IsValid(), "VulkanCommandBuffer must be valid before Begin is called.");
			return vkBeginCommandBuffer(_command_buffer, &command_buffer_begin_info);
		}

		VkResult End()//TODO: should this be in Device??
		{
			NP_ENGINE_ASSERT(IsValid(), "VulkanCommandBuffer must be valid before End is called.");
			return vkEndCommandBuffer(_command_buffer);
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_COMMAND_BUFFER_HPP */
