//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/Commands.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanCommands.hpp"

namespace np::gpu
{
	static inline mem::sptr<Command> CreateVulkanCommand(mem::sptr<srvc::Services> services, CommandType command_type)
	{
		mem::sptr<Command> command = nullptr;

		switch (command_type)
		{
			//copy
		case CommandType::CopyBuffer:
			command = mem::create_sptr<__detail::VulkanCopyBufferCommand>(services->GetAllocator());
			break;
		case CommandType::CopyImage:
			command = mem::create_sptr<__detail::VulkanCopyImageCommand>(services->GetAllocator());
			break;
		case CommandType::CopyBufferToImage:
			command = mem::create_sptr<__detail::VulkanCopyBufferToImageCommand>(services->GetAllocator());
			break;
		case CommandType::CopyImageToBuffer:
			command = mem::create_sptr<__detail::VulkanCopyImageToBufferCommand>(services->GetAllocator());
			break;

			//fills and assigns
		case CommandType::FillBuffer:
			command = mem::create_sptr<__detail::VulkanFillBufferCommand>(services->GetAllocator());
			break;
		case CommandType::AssignBuffer:
			command = mem::create_sptr<__detail::VulkanAssignBufferCommand>(services->GetAllocator());
			break;

			//render pass
		case CommandType::BeginRenderPass:
			command = mem::create_sptr<__detail::VulkanBeginRenderPassCommand>(services->GetAllocator());
			break;
		case CommandType::EndRenderPass:
			command = mem::create_sptr<__detail::VulkanEndRenderPassCommand>(services->GetAllocator());
			break;
		case CommandType::NextSubpass:
			command = mem::create_sptr<__detail::VulkanNextSubpassCommand>(services->GetAllocator());
			break;

			//binds
		case CommandType::BindPipeline:
			command = mem::create_sptr<__detail::VulkanBindPipelineCommand>(services->GetAllocator());
			break;
		case CommandType::BindIndexBuffer:
			command = mem::create_sptr<__detail::VulkanBindIndexBufferCommand>(services->GetAllocator());
			break;
		case CommandType::BindVertexBuffers:
			command = mem::create_sptr<__detail::VulkanBindVertexBuffersCommand>(services->GetAllocator());
			break;
		case CommandType::BindResourceGroup:
			command = mem::create_sptr<__detail::VulkanBindResourceGroupCommand>(services->GetAllocator());
			break;

			//general
		case CommandType::Barrier:
			command = mem::create_sptr<__detail::VulkanBarrierCommand>(services->GetAllocator());
			break;
		case CommandType::ExecuteCommands:
			command = mem::create_sptr<__detail::VulkanExecuteCommandsCommand>(services->GetAllocator());
			break;
		case CommandType::PushData:
			command = mem::create_sptr<__detail::VulkanPushDataCommand>(services->GetAllocator());
			break;

			//set
		case CommandType::SetViewports:
			command = mem::create_sptr<__detail::VulkanSetViewportsCommand>(services->GetAllocator());
			break;
		case CommandType::SetScissors:
			command = mem::create_sptr<__detail::VulkanSetScissorsCommand>(services->GetAllocator());
			break;
		/*
		case CommandType::SetRasterizationDepthBias:
			command = mem::create_sptr<__detail::VulkanSetRasterizationDepthBiasCommand>(services->GetAllocator());
			break;
		case CommandType::SetLineWidth:
			command = mem::create_sptr<__detail::VulkanSetLineWidthCommand>(services->GetAllocator());
			break;
		case CommandType::SetDepthBounds:
			command = mem::create_sptr<__detail::VulkanSetDepthBoundsCommand>(services->GetAllocator());
			break;
		case CommandType::SetBlendConstants:
			command = mem::create_sptr<__detail::VulkanSetBlendConstantsCommand>(services->GetAllocator());
			break;
		case CommandType::SetStencilCompareMask:
			command = mem::create_sptr<__detail::VulkanSetStencilCompareMaskCommand>(services->GetAllocator());
			break;
		case CommandType::SetStencilWriteMask:
			command = mem::create_sptr<__detail::VulkanSetStencilWriteMaskCommand>(services->GetAllocator());
			break;
		case CommandType::SetStencilReferenceValue:
			command = mem::create_sptr<__detail::VulkanSetStencilReferenceValueCommand>(services->GetAllocator());
			break;
		//*/

		//dispath
		case CommandType::Dispatch:
			command = mem::create_sptr<__detail::VulkanDispatchCommand>(services->GetAllocator());
			break;
		case CommandType::IndirectDispatch:
			command = mem::create_sptr<__detail::VulkanIndirectDispatchCommand>(services->GetAllocator());
			break;

			//draw
		case CommandType::Draw:
			command = mem::create_sptr<__detail::VulkanDrawCommand>(services->GetAllocator());
			break;
		case CommandType::DrawIndexed:
			command = mem::create_sptr<__detail::VulkanDrawIndexedCommand>(services->GetAllocator());
			break;
		case CommandType::DrawIndirect:
			command = mem::create_sptr<__detail::VulkanDrawIndirectCommand>(services->GetAllocator());
			break;
		case CommandType::DrawIndexedIndirect:
			command = mem::create_sptr<__detail::VulkanDrawIndexedIndirectCommand>(services->GetAllocator());
			break;
		}

		return command;
	}

	mem::sptr<Command> Command::Create(DetailType detail_type, mem::sptr<srvc::Services> services, CommandType command_type)
	{
		mem::sptr<Command> command = nullptr;

		switch (detail_type)
		{
		case DetailType::Vulkan:
			command = CreateVulkanCommand(services, command_type);
			break;
		}

		return command;
	}
} // namespace np::gpu