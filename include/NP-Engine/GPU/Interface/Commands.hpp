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
#include "SamplerResource.hpp"
#include "Scissor.hpp"
#include "Shader.hpp"
#include "Stage.hpp"
#include "Subview.hpp"
#include "Topology.hpp"
#include "Viewport.hpp"

/*
	TODO: support the following commands:

		- [x] vkCmdPushConstants(
				VkCommandBuffer commandBuffer,
				VkPipelineLayout layout,
				VkShaderStageFlags stageFlags,
				uint32_t offset,
				uint32_t size,
				const void* pValues);

		- [x] vkCmdBeginRenderPass(
				VkCommandBuffer commandBuffer,
				const VkRenderPassBeginInfo * pRenderPassBegin,
				VkSubpassContents contents);

		- [x] vkCmdNextSubpass(
				VkCommandBuffer commandBuffer,
				VkSubpassContents contents);

		- [x] vkCmdEndRenderPass(
				VkCommandBuffer commandBuffer);

		- [x] vkCmdExecuteCommands(
				VkCommandBuffer commandBuffer,
				uint32_t commandBufferCount,
				const VkCommandBuffer * pCommandBuffers);

		- [x] vkCmdCopyBuffer(
				VkCommandBuffer commandBuffer,
				VkBuffer srcBuffer,
				VkBuffer dstBuffer,
				uint32_t regionCount,
				const VkBufferCopy * pRegions);

		- [x] vkCmdCopyImage(
				VkCommandBuffer commandBuffer,
				VkImage srcImage,
				VkImageLayout srcImageLayout,
				VkImage dstImage,
				VkImageLayout dstImageLayout,
				uint32_t regionCount,
				const VkImageCopy * pRegions);

		- [x] vkCmdUpdateBuffer(
				VkCommandBuffer commandBuffer,
				VkBuffer dstBuffer,
				VkDeviceSize dstOffset,
				VkDeviceSize dataSize,
				const void* pData);

		- [x] vkCmdFillBuffer(
				VkCommandBuffer commandBuffer,
				VkBuffer dstBuffer,
				VkDeviceSize dstOffset,
				VkDeviceSize size,
				uint32_t data);

		- [x] vkCmdPipelineBarrier(
				VkCommandBuffer commandBuffer,
				VkPipelineStageFlags srcStageMask,
				VkPipelineStageFlags dstStageMask,
				VkDependencyFlags dependencyFlags,
				uint32_t memoryBarrierCount,
				const VkMemoryBarrier * pMemoryBarriers,
				uint32_t bufferMemoryBarrierCount,
				const VkBufferMemoryBarrier * pBufferMemoryBarriers,
				uint32_t imageMemoryBarrierCount,
				const VkImageMemoryBarrier * pImageMemoryBarriers);

		- [x] vkCmdBindPipeline(
				VkCommandBuffer commandBuffer,
				VkPipelineBindPoint pipelineBindPoint,
				VkPipeline pipeline);

		- [x] vkCmdDispatch(
				VkCommandBuffer commandBuffer,
				uint32_t groupCountX,
				uint32_t groupCountY,
				uint32_t groupCountZ);

		- [x] vkCmdDispatchIndirect(
				VkCommandBuffer commandBuffer,
				VkBuffer buffer,
				VkDeviceSize offset);

		- [x] vkCmdBindDescriptorSets(
				VkCommandBuffer commandBuffer,
				VkPipelineBindPoint pipelineBindPoint,
				VkPipelineLayout layout,
				uint32_t firstSet,
				uint32_t descriptorSetCount,
				const VkDescriptorSet * pDescriptorSets,
				uint32_t dynamicOffsetCount,
				const uint32_t * pDynamicOffsets);

		- [x] vkCmdCopyBufferToImage(
				VkCommandBuffer commandBuffer,
				VkBuffer srcBuffer,
				VkImage dstImage,
				VkImageLayout dstImageLayout,
				uint32_t regionCount,
				const VkBufferImageCopy * pRegions);

		- [x] vkCmdCopyImageToBuffer(
				VkCommandBuffer commandBuffer,
				VkImage srcImage,
				VkImageLayout srcImageLayout,
				VkBuffer dstBuffer,
				uint32_t regionCount,
				const VkBufferImageCopy * pRegions);



		- [x] vkCmdSetViewport(
				VkCommandBuffer commandBuffer,
				uint32_t firstViewport,
				uint32_t viewportCount,
				const VkViewport * pViewports);

		- [x] vkCmdSetScissor(
				VkCommandBuffer commandBuffer,
				uint32_t firstScissor,
				uint32_t scissorCount,
				const VkRect2D * pScissors);

		- [x] vkCmdSetLineWidth(
				VkCommandBuffer commandBuffer,
				float lineWidth);

		- [x] vkCmdSetDepthBias(
				VkCommandBuffer commandBuffer,
				float depthBiasConstantFactor,
				float depthBiasClamp,
				float depthBiasSlopeFactor);

		- [x] vkCmdSetBlendConstants(
				VkCommandBuffer commandBuffer,
				const float blendConstants[4]);

		- [x] vkCmdSetDepthBounds(
				VkCommandBuffer commandBuffer,
				float minDepthBounds,
				float maxDepthBounds);

		- [x] vkCmdSetStencilCompareMask(
				VkCommandBuffer commandBuffer,
				VkStencilFaceFlags faceMask,
				uint32_t compareMask);

		- [x] vkCmdSetStencilWriteMask(
				VkCommandBuffer commandBuffer,
				VkStencilFaceFlags faceMask,
				uint32_t writeMask);

		- [x] vkCmdSetStencilReference(
				VkCommandBuffer commandBuffer,
				VkStencilFaceFlags faceMask,
				uint32_t reference);

		- [x] vkCmdBindIndexBuffer(
				VkCommandBuffer commandBuffer,
				VkBuffer buffer,
				VkDeviceSize offset,
				VkIndexType indexType);

		- [x] vkCmdBindVertexBuffers(
				VkCommandBuffer commandBuffer,
				uint32_t firstBinding,
				uint32_t bindingCount,
				const VkBuffer * pBuffers,
				const VkDeviceSize * pOffsets);

		- [x] vkCmdDraw(
				VkCommandBuffer commandBuffer,
				uint32_t vertexCount,
				uint32_t instanceCount,
				uint32_t firstVertex,
				uint32_t firstInstance);

		- [x] vkCmdDrawIndexed(
				VkCommandBuffer commandBuffer,
				uint32_t indexCount,
				uint32_t instanceCount,
				uint32_t firstIndex,
				int32_t vertexOffset,
				uint32_t firstInstance);

		- [x] vkCmdDrawIndirect(
				VkCommandBuffer commandBuffer,
				VkBuffer buffer,
				VkDeviceSize offset,
				uint32_t drawCount,
				uint32_t stride);

		- [x] vkCmdDrawIndexedIndirect(
				VkCommandBuffer commandBuffer,
				VkBuffer buffer,
				VkDeviceSize offset,
				uint32_t drawCount,
				uint32_t stride);

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






		- [ ] vkCmdSetEvent(
				VkCommandBuffer commandBuffer,
				VkEvent event,
				VkPipelineStageFlags stageMask);

		- [ ] vkCmdResetEvent(
				VkCommandBuffer commandBuffer,
				VkEvent event,
				VkPipelineStageFlags stageMask);

		- [ ] vkCmdWaitEvents(
				VkCommandBuffer commandBuffer,
				uint32_t eventCount,
				const VkEvent * pEvents,
				VkPipelineStageFlags srcStageMask,
				VkPipelineStageFlags dstStageMask,
				uint32_t memoryBarrierCount,
				const VkMemoryBarrier * pMemoryBarriers,
				uint32_t bufferMemoryBarrierCount,
				const VkBufferMemoryBarrier * pBufferMemoryBarriers,
				uint32_t imageMemoryBarrierCount,
				const VkImageMemoryBarrier * pImageMemoryBarriers);



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

	struct CopyImageRange
	{
		ImageResource::Point dstOffset{};
		ImageResource::Point srcOffset{};
		ImageResource::Point size{}; //width and height to copy
	};

	struct CopyImageCommand : public Command
	{
		mem::sptr<ImageResource> dst = nullptr;
		mem::sptr<ImageResource> src = nullptr;
		con::vector<CopyImageRange> ranges{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::CopyImage;
		}
	};

	struct CopyBufferToImageRange
	{
		ImageResource::Point dstOffset{};
		//TODO: should we consider VkImageSubresourceLayers?

		siz srcOffset = 0;
		//ui32 srcImageWidth = 0; //TODO: feel like the image provides this?
		//ui32 srcImageHeight = 0; //TODO: feel like the image provides this?

		siz size = 0;
	};

	struct CopyBufferToImageCommand : public Command
	{
		mem::sptr<ImageResource> dst;
		mem::sptr<BufferResource> src;
		con::vector<CopyBufferToImageRange> ranges;
		//TODO: VkImageLayout dstImageLayout ??

		virtual CommandType GetCommandType() const override
		{
			return CommandType::CopyBufferToImage;
		}
	};

	struct CopyImageToBufferRange
	{
		siz dstOffset = 0;
		//ui32 dstImageWidth = 0; //TODO: feel like the image provides this?
		//ui32 dstImageHeight = 0; //TODO: feel like the image provides this?

		//TODO: I feel like we need a begin and end point, but also a good way to indicate that we just want what is in these
		//bounds, or if we want all image data from that point, then sequentially through the image until we reach the end point
		Image::Point srcBeginPoint{};
		//TODO: should we consider VkImageSubresourceLayers?

		Image::Point srcEndPoint{};

		//siz size = 0; //TODO: should we replace this with a width/height bounds for the image?
	};

	struct CopyImageToBufferCommand : public Command
	{
		mem::sptr<BufferResource> dst;
		mem::sptr<ImageResource> src;
		con::vector<CopyImageToBufferRange> ranges;
		//TODO: VkImageLayout dstImageLayout ??

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
		siz value = 0;
		//siz valueSize = sizeof(ui32); //default to size of ui32, but user may want size with [sizeof(ui8), sizeof(siz)]

		virtual CommandType GetCommandType() const override
		{
			return CommandType::FillBuffer;
		}
	};

	struct AssignBufferCommand : public Command
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz offset = 0; //within buffer
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
		con::vector<ClearColor> clearColors{}; //size = [1, N) where N = number of images in framebuffer?
		SubpassUsage usage =
			SubpassUsage::None; //TODO: some way to indicate whether to not we're enabling secondary command buffers

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
		//TODO: subpass stuff?

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

	struct BufferBarrier : public Barrier
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz offset = 0;
		siz size = 0; //SIZ_MAX equates to the remaining portion of the buffer after offset
		DeviceQueueFamily dstQueueFamily{};
		DeviceQueueFamily srcQueueFamily{};
	};

	struct ImageBarrier : public Barrier
	{
		mem::sptr<ImageResource> image = nullptr;
		//ImageResourceUsage dstImageResourceUsage = ImageResourceUsage::None;
		//ImageResourceUsage srcImageResourceUsage = ImageResourceUsage::None;
		//TODO: add image layout fields?
		//TODO: add subresource range field?
		DeviceQueueFamily dstQueueFamily{};
		DeviceQueueFamily srcQueueFamily{};
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

	struct BindResourceGroupCommand : public Command
	{
		/*
		- [x] vkCmdBindDescriptorSets(
				VkCommandBuffer commandBuffer,
				VkPipelineBindPoint pipelineBindPoint,
				VkPipelineLayout layout,
				uint32_t firstSet,
				uint32_t descriptorSetCount,
				const VkDescriptorSet * pDescriptorSets,
				uint32_t dynamicOffsetCount,
				const uint32_t * pDynamicOffsets);
		*/

		mem::sptr<Pipeline> pipeline = nullptr;
		siz resourceLayoutIndex =
			SIZ_MAX; //index into the PipelineResourceLayout's ResourceLayouts the given group will apply to
		mem::sptr<ResourceGroup> group =
			nullptr; //TODO: always check and make sure this group is compatible with the resource layout in the pipeline

		//TODO: some resources are dynamic and need offsets. We are required to consider them. Doesn't have to be here, but
		//needs to happpen.
		con::vector<siz> dynamicResourceOffsets{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::BindResourceGroup;
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
		//siz index = 0; //TODO: I don't think we need this.
		con::vector<Viewport> viewports{};

		virtual CommandType GetCommandType() const override
		{
			return CommandType::SetViewports;
		}
	};

	struct SetScissorsCommand : public Command
	{
		//siz index = 0; //TODO: I don't think we need this.
		con::vector<Scissor> scissors{};

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

	//struct BindIndexBufferCommand : public Command
	//{
	//	mem::sptr<BufferResource> buffer = nullptr;
	//	siz offset = 0;
	//	siz stride = 0; //common strides: sizeof(ui16), sizeof(ui32)

	//	virtual CommandType GetCommandType() const override
	//	{
	//		return CommandType::BindIndexBuffer;
	//	}
	//};

	//struct BindVertexBuffersCommand : public Command
	//{
	//	con::vector<mem::sptr<BufferResource>> buffers{};
	//	con::vector<siz> offsets{};

	//	virtual CommandType GetCommandType() const override
	//	{
	//		return CommandType::BindVertexBuffers;
	//	}
	//};

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
		siz instanceCount = 0;
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
		siz instanceCount = 0; //TODO: does this have to default to 1? investigate
		siz instanceBeginIndex = 0;
	};

	struct DrawIndirectCommand : public Command
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz payloadOffset = 0;
		siz payloadCount = 0;
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
		siz vertexOffset = 0; //TODO: should this be i64?
		siz instanceCount = 0; //TODO: does this have to default to 1? investigate
		siz instanceBeginIndex = 0;
	};

	struct DrawIndexedIndirectCommand : public Command
	{
		mem::sptr<BufferResource> buffer = nullptr;
		siz payloadOffset = 0;
		siz payloadCount = 0;
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