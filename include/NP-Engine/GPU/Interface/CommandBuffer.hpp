//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/9/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_COMMAND_BUFFER_HPP
#define NP_ENGINE_GPU_INTERFACE_COMMAND_BUFFER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Detail.hpp"

namespace np::gpu
{
	enum class CommandType : ui32
	{
		None = 0,

		//copy
		CopyBuffer,
		CopyImage,
		CopyBufferToImage,
		CopyImageToBuffer, //TODO: we need this too

		//fills and assigns
		FillBuffer,
		AssignBuffer,

		//render pass
		BeginRenderPass,
		EndRenderPass,
		NextSubpass, //TODO: are we in charge of this? feels like a vulkan thing

		//binds
		BindPipeline,
		BindIndexBuffer,
		BindVertexBuffers,
		BindResourceGroup,

		//general
		Barrier,
		ExecuteCommands,
		PushData,

		//set
		SetViewports,
		SetScissors,
		SetRasterizationDepthBias,
		SetLineWidth,
		SetDepthBounds,
		SetBlendConstants,
		SetStencilCompareMask,
		SetStencilWriteMask,
		SetStencilReferenceValue,

		//dispatch
		Dispatch,
		IndirectDispatch,

		//draw
		Present, //TODO: Do we need this?? Feel like this should be for the PresentQueue object?
		Draw,
		DrawIndexed,
		DrawIndirect,
		DrawIndexedIndirect,
	};

	class CommandBuffer;

	class Command : public DetailObject //ONLY inherit this virtually for our commands
										//<https://en.cppreference.com/w/cpp/language/derived_class.html>
	{
	protected:
		friend class CommandBuffer;

		virtual bl ApplyTo(const CommandBuffer* command_buffer) = 0;

	public:
		virtual ~Command() = default;

		static mem::sptr<Command> Create(DetailType detail_type, mem::sptr<srvc::Services> services, CommandType command_type);

		virtual CommandType GetCommandType() const = 0;

		/*
			some commands require preparations before being applied to a command buffer
			this method indicates if the command is properly prepared
		*/
		virtual bl IsPrepared() const = 0; //TODO: mainly indirect commands need preparations -- ensure all do
	};

	class CommandBufferUsage : public Enum<ui32>
	{
	public:
		constexpr static ui32 SingleUse = BIT(0);

		CommandBufferUsage(ui32 value): Enum<ui32>(value) {}
	};

	struct CommandBuffer : public DetailObject
	{
		virtual ~CommandBuffer() = default;

		virtual bl Add(mem::sptr<Command> command)
		{
			return command && command->ApplyTo(this);
		}

		virtual bl DependOn(mem::sptr<CommandBuffer> other) = 0;

		virtual con::vector<mem::sptr<CommandBuffer>> GetDependencies() const = 0;

		virtual bl HasDependecies() const
		{
			return !GetDependencies().empty();
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_COMMAND_BUFFER_HPP */