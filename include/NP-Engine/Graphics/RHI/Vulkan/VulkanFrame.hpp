//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_FRAME_HPP
#define NP_ENGINE_VULKAN_FRAME_HPP

#include <iostream> //TODO: remove
#include <utility>

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"

namespace np::graphics::rhi
{
	class VulkanFrame : public Frame
	{
	private:
		VulkanCommandBuffer _command_buffer;
		VulkanDevice* _device;
		container::vector<VulkanCommand*> _commands;

	public:
		VulkanFrame()
		{
			Invalidate();
		}

		VulkanFrame(VulkanDevice& device, VulkanCommandBuffer& command_buffer):
			_device(memory::AddressOf(device)),
			_command_buffer(command_buffer)
		{}

		VulkanFrame(const VulkanFrame& other):
			_device(other._device),
			_command_buffer(other._command_buffer),
			_commands(other._commands)
		{}

		VulkanFrame(VulkanFrame&& other) noexcept:
			_device(::std::move(other._device)),
			_command_buffer(::std::move(other._command_buffer)),
			_commands(::std::move(other._commands))
		{
			other.Invalidate();
		}

		VulkanFrame& operator=(const VulkanFrame& other)
		{
			_command_buffer = other._command_buffer;
			_device = other._device;
			_commands = other._commands;
			return *this;
		}

		VulkanFrame& operator=(VulkanFrame&& other) noexcept
		{
			_command_buffer = ::std::move(other._command_buffer);
			_device = ::std::move(other._device);
			_commands = ::std::move(other._commands);
			other.Invalidate();
			return *this;
		}

		bl IsValid() const override
		{
			return _command_buffer.IsValid() && _device != nullptr;
		}

		void Invalidate() override
		{
			_command_buffer.Invalidate();
			_commands.clear();
			_device = nullptr;
		}

		void Begin(VkCommandBufferBeginInfo& command_buffer_begin_info)
		{
			NP_ENGINE_ASSERT(IsValid(), "frame must be valid to call this.");
			_command_buffer.Begin(command_buffer_begin_info);
		}

		void End()
		{
			NP_ENGINE_ASSERT(IsValid(), "frame must be valid to call this.");
			_command_buffer.End();
		}

		VulkanDevice& GetDevice()
		{
			NP_ENGINE_ASSERT(IsValid(), "frame must be valid to call this.");
			return *_device;
		}

		const VulkanDevice& GetDevice() const
		{
			NP_ENGINE_ASSERT(IsValid(), "frame must be valid to call this.");
			return *_device;
		}

		siz GetStagedSlot()
		{
			siz slot = _commands.size();
			_commands.emplace_back(nullptr);
			return slot;
		}

		void StageCommand(VulkanCommand& command)
		{
			_commands.emplace_back(memory::AddressOf(command));
		}

		void StageCommand(siz slot, VulkanCommand& command)
		{
			_commands[slot] = memory::AddressOf(command);
		}

		void SubmitStagedCommandsToBuffer()
		{
			NP_ENGINE_ASSERT(IsValid(), "frame must be valid to call this.");
			for (VulkanCommand*& command : _commands)
				if (command != nullptr)
					_command_buffer.Add(*command);
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_FRAME_HPP */