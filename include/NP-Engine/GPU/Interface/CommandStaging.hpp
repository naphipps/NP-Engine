//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/14/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_COMMAND_STAGING_HPP
#define NP_ENGINE_GPU_INTERFACE_COMMAND_STAGING_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "CommandBuffer.hpp"

// TODO: do we want a timestamp and timestamp diff from last timestamp? I think so...?

namespace np::gpu
{
	class CommandStaging
	{
	protected:
		mem::sptr<CommandBuffer> _command_buffer;
		con::vector<mem::sptr<Command>> _commands;

	public:
		CommandStaging() : _command_buffer(nullptr) {}

		CommandStaging(nptr) : CommandStaging() {}

		CommandStaging(mem::sptr<CommandBuffer> command_buffer) : _command_buffer(command_buffer) {}

		CommandStaging(CommandStaging&& other) noexcept :
			_command_buffer(::std::move(other._command_buffer)),
			_commands(::std::move(other._commands))
		{
			other.Invalidate();
		}

		CommandStaging& operator=(CommandStaging&& other) noexcept
		{
			_command_buffer = ::std::move(other._command_buffer);
			_commands = ::std::move(other._commands);
			other.Invalidate();
			return *this;
		}

		virtual bl IsValid() const
		{
			return _command_buffer;
		}

		virtual void Invalidate()
		{
			_commands.clear();
			_command_buffer.reset();
		}

		mem::sptr<CommandBuffer> GetCommandBuffer() const
		{
			return _command_buffer;
		}

		virtual siz GetStagedSlot()
		{
			siz slot = _commands.size();
			Stage(nullptr);
			return slot;
		}

		virtual void Stage(mem::sptr<Command> command)
		{
			_commands.emplace_back(command);
		}

		virtual void Stage(siz slot, mem::sptr<Command> command)
		{
			if (slot < _commands.size())
				_commands[slot] = command;
		}

		virtual void DigestCommands()
		{
			NP_ENGINE_ASSERT(IsValid(), "command staging must be valid to call this.");
			for (mem::sptr<Command> command : _commands)
				if (command)
					_command_buffer->Add(*command);
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_COMMAND_STAGING_HPP */