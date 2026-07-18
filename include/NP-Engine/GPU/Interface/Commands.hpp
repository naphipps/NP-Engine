//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_COMMANDS_HPP
#define NP_ENGINE_GPU_INTERFACE_COMMANDS_HPP

#include "NP-Engine/Math/Math.hpp"

#include "Access.hpp"
#include "Blend.hpp"
#include "BufferResource.hpp"
#include "Camera.hpp"
#include "Color.hpp"
#include "ComputePipeline.hpp"
#include "CommandBuffer.hpp"
#include "DepthStencil.hpp"
#include "Detail.hpp"
#include "DmsImage.hpp"
#include "DmsLineSegment.hpp"
#include "DrawableImage.hpp"
#include "Dynamic.hpp"
#include "Flag.hpp"
#include "FloodFillImage.hpp"
#include "Framebuffer.hpp"
#include "FrameContext.hpp"
#include "GraphicsPipeline.hpp"
#include "Image.hpp"
#include "ImageResource.hpp"
#include "ImageSubview.hpp"
#include "Light.hpp"
#include "Logic.hpp"
#include "Model.hpp"
#include "Multisample.hpp"
#include "Pipeline.hpp"
#include "PipelineCache.hpp"
#include "PresentTarget.hpp"
#include "PushData.hpp"
#include "Queue.hpp"
#include "Rasterization.hpp"
#include "RenderPass.hpp"
#include "Resource.hpp"
#include "ResourceGroup.hpp"
#include "SamplerResource.hpp"
#include "Scissor.hpp"
#include "Shader.hpp"
#include "Stage.hpp"
#include "Subview.hpp"
#include "Topology.hpp"
#include "Viewport.hpp"

/*
	TODO: support the following commands:

		--------------------------------------------------------------

		- [ ] vkCmdBlitImage(
				VkCommandBuffer commandBuffer,
				VkImage srcImage,
				VkImageLayout srcImageLayout,
				VkImage dstImage,
				VkImageLayout dstImageLayout,
				uint32_t regionCount,
				const VkImageBlit * pRegions,
				VkFilter filter);

		- [ ] vkCmdClearColorImage(
				VkCommandBuffer commandBuffer,
				VkImage image,
				VkImageLayout imageLayout,
				const VkClearColorValue * pColor,
				uint32_t rangeCount,
				const VkImageSubresourceRange * pRanges);

		- [ ] vkCmdClearDepthStencilImage(
				VkCommandBuffer commandBuffer,
				VkImage image,
				VkImageLayout imageLayout,
				const VkClearDepthStencilValue * pDepthStencil,
				uint32_t rangeCount,
				const VkImageSubresourceRange * pRanges);

		- [ ] vkCmdResolveImage(
				VkCommandBuffer commandBuffer,
				VkImage srcImage, //srcImage
				VkImageLayout srcImageLayout, //srcImage
				VkImage dstImage, //dstImage
				VkImageLayout dstImageLayout, //dstImage, or should we accept a ImageResourceUsage, and we allow asigning a
   image it's usage after creation? maybe thats a detail thing uint32_t regionCount, //regions const VkImageResolve * pRegions);
   //regions


		--------------------------------------------------------------

		- [ ] vkCmdClearAttachments(
				VkCommandBuffer commandBuffer,
				uint32_t attachmentCount,
				const VkClearAttachment * pAttachments,
				uint32_t rectCount,
				const VkClearRect * pRects);

		- [ ] vkCmdBeginQuery(
				VkCommandBuffer commandBuffer,
				VkQueryPool queryPool,
				uint32_t query,
				VkQueryControlFlags flags);

		- [ ] vkCmdEndQuery(
				VkCommandBuffer commandBuffer,
				VkQueryPool queryPool,
				uint32_t query);

		- [ ] vkCmdResetQueryPool(
				VkCommandBuffer commandBuffer,
				VkQueryPool queryPool,
				uint32_t firstQuery,
				uint32_t queryCount);

		- [ ] vkCmdWriteTimestamp(
				VkCommandBuffer commandBuffer,
				VkPipelineStageFlagBits pipelineStage,
				VkQueryPool queryPool,
				uint32_t query);

		- [ ] vkCmdCopyQueryPoolResults(
				VkCommandBuffer commandBuffer,
				VkQueryPool queryPool,
				uint32_t firstQuery,
				uint32_t queryCount,
				VkBuffer dstBuffer,
				VkDeviceSize dstOffset,
				VkDeviceSize stride,
				VkQueryResultFlags flags);
*/

//TODO: we need to determine good default values for everything

namespace np::gpu
{
	struct CopyBufferRange
	{
		siz dstOffset = 0;
		siz srcOffset = 0;
		siz size = 0;
	};

	struct CopyBufferCommand : public Command
	{
		mem::sptr<BufferResource> dst = nullptr;
		mem::sptr<BufferResource> src = nullptr;
		con::vector<CopyBufferRange> ranges{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::CopyBuffer;
		}
	};

	struct ImageLayers
	{
		ImageResourceUsage usage = ImageResourceUsage::None;
		siz mipLevel = 0;
		siz layerCount = 1;
		siz layerBeginIndex = 0; //TODO: add better support for image arrays
	};

	struct ImageRange
	{
		ImageResourceUsage usage = ImageResourceUsage::None;
		siz mipCount = 1;
		siz mipBeginIndex = 0;
		siz layerCount = 1;
		siz layerBeginIndex = 0; //TODO: add better support for image arrays
	};

	struct CopyImageContext
	{
		ImageResource::Point dstOffset{};
		ImageResource::Point srcOffset{};
		ImageResource::Point size{};
		ImageLayers dstLayers{};
		ImageLayers srcLayers{};
	};

	struct CopyImageCommand : public Command
	{
		mem::sptr<ImageResource> dst = nullptr;
		mem::sptr<ImageResource> src = nullptr;
		ImageResourceUsage dstUsage = ImageResourceUsage::None;
		ImageResourceUsage srcUsage = ImageResourceUsage::None;
		con::vector<CopyImageContext> contexts{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::CopyImage;
		}
	};

	struct CopyBufferToImageRange
	{
		ImageResource::Point imageOffset{};
		ImageResource::Point imageSize{};
		ImageLayers imageLayers{};
		siz bufferOffset = 0;
		siz bufferRowCount = 0;
		siz bufferRowLength = 0;
	};

	struct CopyBufferToImageCommand : public Command
	{
		mem::sptr<ImageResource> image = nullptr;
		ImageResourceUsage imageUsage = ImageResourceUsage::None;
		mem::sptr<BufferResource> buffer = nullptr;
		con::vector<CopyBufferToImageRange> ranges{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::CopyBufferToImage;
		}
	};

	using CopyImageToBufferRange = CopyBufferToImageRange;

	struct CopyImageToBufferCommand : public Command
	{
		mem::sptr<ImageResource> image = nullptr;
		ImageResourceUsage imageUsage = ImageResourceUsage::None;
		mem::sptr<BufferResource> buffer = nullptr;
		con::vector<CopyImageToBufferRange> ranges{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::CopyImageToBuffer;
		}
	};

	struct FillBufferCommand : public Command
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz offset = 0;
		siz count = 0;
		ui32 value = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::FillBuffer;
		}
	};

	struct AssignBufferCommand : public Command
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz offset = 0;
		con::vector<ui8> values{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::AssignBuffer;
		}
	};

	struct RenderArea
	{
		::glm::i64vec2 offset{};
		siz width = 0;
		siz height = 0;
	};

	struct BeginRenderPassCommand : public Command
	{
		mem::sptr<Framebuffer> framebuffer = nullptr;
		RenderArea renderArea{};
		//one for each image view in framebuffer
		con::vector<ClearColor> clearColors{}; 
		
		SubpassUsage usage = SubpassUsage::None; 
		//TODO: some way to indicate whether or not we're enabling secondary command buffers

		virtual CommandType GetCommandType() const override
		{
			return CommandType::BeginRenderPass;
		}
	};

	struct EndRenderPassCommand : public Command
	{
		mem::sptr<Framebuffer> framebuffer = nullptr;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::EndRenderPass;
		}
	};

	struct NextSubpassCommand : public Command
	{
		SubpassUsage usage = SubpassUsage::None;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::NextSubpass;
		}
	};

	struct BindPipelineCommand : public Command
	{
		mem::sptr<Pipeline> pipeline = nullptr;
		PipelineUsage usage = PipelineUsage::None;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::BindPipeline;
		}
	};

	struct BindIndexBufferCommand : public Command
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz offset = 0;
		siz stride = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::BindIndexBuffer;
		}
	};

	struct BindVertexBufferContext
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz offset = 0;
	};

	struct BindVertexBuffersCommand : public Command
	{
		con::vector<BindVertexBufferContext> contexts{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::BindVertexBuffers;
		}
	};

	/*
		TODO: make a better comment here
		execute given command buffers on the given command buffer

		TODO: investigate nested command buffers:
	   <https://docs.vulkan.org/spec/latest/chapters/features.html#features-nestedCommandBuffer>
	*/
	struct ExecuteCommandsCommand : public Command
	{
		con::vector<mem::sptr<CommandBuffer>> commandBuffers{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::ExecuteCommands;
		}
	};

	struct Barrier
	{
		Access dstAccess = Access::None;
		Access srcAccess = Access::None;
	};

	struct BufferBarrier : public Barrier //TODO: investigate removing this inheritance
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz offset = 0;
		siz size = 0; //SIZ_MAX equates to the remaining portion of the buffer after offset
		DeviceQueueFamily dstQueueFamily{};
		DeviceQueueFamily srcQueueFamily{};
	};

	struct ImageBarrier : public Barrier //TODO: investigate removing this inheritance
	{
		mem::sptr<ImageResource> image = nullptr;
		ImageResourceUsage dstImageResourceUsage = ImageResourceUsage::None;
		ImageResourceUsage srcImageResourceUsage = ImageResourceUsage::None;
		DeviceQueueFamily dstQueueFamily{};
		DeviceQueueFamily srcQueueFamily{};
		ImageRange range{};
	};

	struct BarrierCommand : public Command
	{
		Stage dstStage = Stage::None;
		Stage srcStage = Stage::None;
		con::vector<Barrier> barriers{};
		con::vector<BufferBarrier> bufferBarriers{};
		con::vector<ImageBarrier> imageBarriers{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::Barrier;
		}
	};

	struct BindResourceGroupsCommand : public Command
	{
		mem::sptr<Pipeline> pipeline = nullptr;
		siz resourceLayoutBeginIndex = 0;
		con::vector<mem::sptr<ResourceGroup>> resourceGroups{};
		con::vector<siz> dynamicResourceOffsets{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::BindResourceGroups;
		}
	};

	struct PushDataCommand : public Command
	{
		mem::sptr<Pipeline> pipeline = nullptr;
		Stage stage = Stage::None;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::PushData;
		}
	};

	struct SetViewportsCommand : public Command
	{
		con::vector<Viewport> viewports{};
		siz viewportBeginIndex = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetViewports;
		}
	};

	struct SetScissorsCommand : public Command
	{
		con::vector<Scissor> scissors{};
		siz scissorBeginIndex = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetScissors;
		}
	};

	struct SetLineWidthCommand : public Command
	{
		dbl width = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetLineWidth;
		}
	};

	struct SetRasterizationDepthBiasCommand : public Command
	{
		DepthBias bias{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetRasterizationDepthBias;
		}
	};

	struct SetBlendConstantsCommand : public Command
	{
		BlendConstants blendConstants{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetBlendConstants;
		}
	};

	struct SetDepthBoundsCommand : public Command
	{
		DepthBounds depthBounds{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetDepthBounds;
		}
	};

	struct SetStencilCompareMaskCommand : public Command
	{
		StencilFace face = StencilFace::None;
		siz mask = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetStencilCompareMask;
		}
	};

	struct SetStencilWriteMaskCommand : public Command
	{
		StencilFace face = StencilFace::None;
		siz mask = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetStencilWriteMask;
		}
	};

	struct SetStencilReferenceValueCommand : public Command
	{
		StencilFace face = StencilFace::None;
		siz referenceValue = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetStencilReferenceValue;
		}
	};

	struct DispatchCommand : public Command
	{
		siz x = 0;
		siz y = 0;
		siz z = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::Dispatch;
		}
	};

	struct IndirectDispatchCommand : public Command
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz offset = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::IndirectDispatch;
		}
	};

	struct WaitFlagsCommand : public Command
	{
		con::vector<mem::sptr<Flag>> flags{};
		Stage dstStage = Stage::None;
		Stage srcStage = Stage::None;
		con::vector<Barrier> barriers{};
		con::vector<BufferBarrier> bufferBarriers{};
		con::vector<ImageBarrier> imageBarriers{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::WaitFlags;
		}
	};

	struct SetFlagCommand : public Command
	{
		mem::sptr<Flag> flag = nullptr;
		Stage stage = Stage::None;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetFlag;
		}
	};

	struct ResetFlagCommand : public Command
	{
		mem::sptr<Flag> flag = nullptr;
		Stage stage = Stage::None;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::ResetFlag;
		}
	};

	struct DrawCommand : public Command
	{
		siz vertexCount = 0;
		siz vertexBeginIndex = 0;
		siz instanceCount = 1;
		siz instanceBeginIndex = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::Draw;
		}
	};

	struct DrawIndexedCommand : public Command //TODO: investigate index-only renderering (pg 276)
	{
		siz indexCount = 0;
		siz indexBeginIndex = 0;
		siz vertexOffset = 0;
		siz instanceCount = 1;
		siz instanceBeginIndex = 0;

		virtual CommandType GetCommandType() const override
		{
			return CommandType::DrawIndexed;
		}
	};

	/*
		TODO: investigate using gl_DrawIDARB to indicate which draw call you are within in a shader (pg 285-286)
		TODO: ^ "#extension GL_ARB_shader_draw_paramters : require"
		TODO: ^ sacha willems indirect example do NOT use this, so we may not use this, but use other means
	*/

	struct DrawIndirectCommandPayload
	{
		siz vertexCount = 0;
		siz vertexBeginIndex = 0;
		siz instanceCount = 1; //TODO: does this have to default to 1? investigate
		siz instanceBeginIndex = 0;
	};

	struct DrawIndirectCommand : public Command
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz payloadOffset = 0;
		siz payloadCount = 0; //TODO: I don't think we need this... investigate
		siz drawCount = 1; //if payloadCount < drawCount, then only the first payload will be used for the repeated draw calls

		virtual CommandType GetCommandType() const override
		{
			return CommandType::DrawIndirect;
		}

		/*
			required to call this before adding to a command buffer
			this methods sets payloadOffset and payloadCount
		*/
		virtual bl Prepare(siz offset, const con::vector<DrawIndirectCommandPayload>& payloads) = 0;
	};

	struct DrawIndexedIndirectCommandPayload
	{
		siz indexCount = 0;
		siz indexBeginIndex = 0;
		siz vertexOffset = 0;
		siz instanceCount = 1;
		siz instanceBeginIndex = 0;
	};

	struct DrawIndexedIndirectCommand : public Command
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz payloadOffset = 0;
		siz payloadCount = 0; //TODO: I don't think we need this... investigate
		siz drawCount = 1; //if payloadCount < drawCount, then only the first payload will be used for the repeated draw calls
		//TODO: ^ some devices may not support multiple draws (look at device features pg 285)
		//TODO: ^ devices that support multiple draws will have a device limit

		virtual CommandType GetCommandType() const override
		{
			return CommandType::DrawIndexedIndirect;
		}

		/*
			required to call this before adding to a command buffer
			this methods sets payloadOffset and payloadCount
		*/
		virtual bl Prepare(siz offset, const con::vector<DrawIndexedIndirectCommandPayload>& payloads) = 0;
	};

} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_COMMANDS_HPP */