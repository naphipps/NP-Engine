//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/2/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_COMMAND_BUFFER_POOL_HPP
#define NP_ENGINE_GPU_INTERFACE_COMMAND_BUFFER_POOL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "CommandBuffer.hpp"
#include "Detail.hpp"

namespace np::gpu
{
	class CommandBufferPoolUsage : public enm_ui32
	{
	public:
		constexpr static ui32 Transient = BIT(0);
		constexpr static ui32 Resettable = BIT(1);

		CommandBufferPoolUsage(ui32 value): enm_ui32(value) {}
	};

	struct CommandBufferPool : public DetailObject
	{
		virtual ~CommandBufferPool() {}

		virtual mem::sptr<CommandBuffer> CreateCommandBuffer() = 0;

		virtual bl Begin(mem::sptr<CommandBuffer> command_buffer, CommandBufferUsage usage) = 0;

		virtual bl End(mem::sptr<CommandBuffer> command_buffer, CommandBufferUsage usage) = 0;

		virtual bl Reset(mem::sptr<CommandBuffer> command_buffer, CommandBufferUsage usage) = 0;
	};
} //namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_COMMAND_BUFFER_POOL_HPP */