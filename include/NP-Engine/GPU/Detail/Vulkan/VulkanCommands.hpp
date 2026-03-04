//##===----------------------------------------------------------------------===##//
//
// Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_COMMANDS_HPP
#define NP_ENGINE_GPU_VULKAN_COMMANDS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

//TODO: I think this file could use some simplifying

#include "VulkanStage.hpp"
#include "VulkanBufferResource.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanComputePipeline.hpp"
#include "VulkanGraphicsPipeline.hpp"
#include "VulkanAccess.hpp"
#include "VulkanQueue.hpp"
#include "VulkanImageResource.hpp"
#include "VulkanViewport.hpp"
#include "VulkanScissor.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanColor.hpp"
#include "VulkanCommandBuffer.hpp"

namespace np::gpu::__detail
{
	struct VulkanCommand;

	struct VulkanCopyBufferRange
	{
		siz dstOffset = 0;
		siz srcOffset = 0;
		siz size = 0;

		VulkanCopyBufferRange(const CopyBufferRange& other = {}) :
			dstOffset(other.dstOffset),
			srcOffset(other.srcOffset),
			size(other.size)
		{}

		operator CopyBufferRange() const
		{
			return { dstOffset, srcOffset, size };
		}

		VkBufferCopy GetVkBufferCopy() const
		{
			return { srcOffset, dstOffset, size };
		}
	};

	class VulkanCopyBufferCommand : public CopyBufferCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			mem::sptr<VulkanBufferResource> dst = EnsureIsDetailType(this->dst, DetailType::Vulkan);
			mem::sptr<VulkanBufferResource> src = EnsureIsDetailType(this->src, DetailType::Vulkan);

			con::vector<VkBufferCopy> ranges(this->ranges.size());
			for (siz i = 0; i < ranges.size(); i++)
				ranges[i] = VulkanCopyBufferRange{ this->ranges[i] }.GetVkBufferCopy();

			vkCmdCopyBuffer(*command_buffer, *src, *dst, ranges.size(), ranges.empty() ? nullptr : ranges.data());
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanCopyImageCommand : public CopyImageCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			//vkCmdCopyImage(command_buffer, src, src_layout, dst, dst_layout, ranges.size, ranges.data); //TODO: implement
			return false;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanCopyBufferToImageCommand : public CopyBufferToImageCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			//vkCmdCopyBufferToImage(command_buffer, srcBuffer, dstImage, dstImageLayout, (ui32)regions.size(), regions.data()); //TODO: implement
			return false;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanCopyImageToBufferCommand : public CopyImageToBufferCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			//vkCmdCopyImageToBuffer(command_buffer, src, src_layout, dst, ranges.size, ranges.data); //TODO: implement
			return false;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanFillBufferCommand : public FillBufferCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);

			bl applied = true;
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			if (buffer)
			{
				//TODO: ensure the filled region is within the buffer
				vkCmdFillBuffer(*command_buffer, *buffer, offset, count, value);
				applied = true;
			}
			return applied;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanAssignBufferCommand : public AssignBufferCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);

			bl applied = true;
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			if (buffer)
			{
				//TODO: ensure the updated region is within the buffer
				vkCmdUpdateBuffer(*command_buffer, *buffer, offset, values.size(), values.empty() ? nullptr : values.data());
				applied = true;
			}
			return applied;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};


	/*typedef struct VkOffset2D {
		int32_t    x;
		int32_t    y;
	} VkOffset2D;

	typedef struct VkOffset3D {
		int32_t    x;
		int32_t    y;
		int32_t    z;
	} VkOffset3D;

	typedef struct VkRect2D {
		VkOffset2D    offset;
		VkExtent2D    extent;
	} VkRect2D;*/


	struct VulkanRenderArea
	{
		::glm::i32vec2 offset;
		ui32 width;
		ui32 height;

		VulkanRenderArea(const RenderArea& other = {}):
			offset(other.offset),
			width(other.width),
			height(other.height)
		{}

		operator RenderArea() const
		{
			return { offset, width, height };
		}

		VkRect2D GetVkRect2D() const
		{
			return { {offset.x, offset.y}, {width, height} };
		}
	};


	class VulkanBeginRenderPassCommand : public BeginRenderPassCommand
	{
	protected:
		static VkRenderPassBeginInfo GetVkRenderPassBeginInfo(mem::sptr<VulkanFramebuffer> framebuffer, VkRect2D render_area)
		{
			mem::sptr<VulkanRenderPass> renderpass = EnsureIsDetailType(framebuffer->GetRenderPass(), DetailType::Vulkan);

			VkRenderPassBeginInfo info{};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.framebuffer = *framebuffer;
			info.renderPass = *renderpass;
			info.renderArea = render_area;
			return info;
		}

		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);

			bl applied = false;
			mem::sptr<VulkanFramebuffer> framebuffer = EnsureIsDetailType(this->framebuffer, DetailType::Vulkan);

			if (framebuffer)
			{
				const VulkanSubpassUsage usage = this->usage;
				const VulkanRenderArea render_area = renderArea;

				con::vector<VkClearValue> clear_values(clearColors.size());
				for (siz i = 0; i < clear_values.size(); i++)
					clear_values[i] = { VulkanClearColor{ clearColors[i] }.GetVkClearColorValue() };

				VkRenderPassBeginInfo info = GetVkRenderPassBeginInfo(framebuffer, render_area.GetVkRect2D());
				info.clearValueCount = clear_values.size();
				info.pClearValues = clear_values.empty() ? nullptr : clear_values.data();

				vkCmdBeginRenderPass(*command_buffer, &info, usage.GetVkSubpassContents()); //TODO: implement
				applied = true;
			}
			return applied;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanEndRenderPassCommand : public EndRenderPassCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			vkCmdEndRenderPass(*command_buffer);
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanNextSubpassCommand : public NextSubpassCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			//vkCmdNextSubpass(command_buffer, subpass_contents); //TODO: implement
			return false;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanBindPipelineCommand : public BindPipelineCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			bl applied = false;

			VkPipeline vk_pipeline = nullptr;

			switch (pipeline->GetPipelineType())
			{
			case PipelineType::Compute:
			{
				mem::sptr<VulkanComputePipeline> vulkan_pipeline = EnsureIsDetailType(pipeline, DetailType::Vulkan);
				vk_pipeline = *vulkan_pipeline;
				break;
			}
			case PipelineType::Graphics:
			{
				mem::sptr<VulkanGraphicsPipeline> vulkan_pipeline = EnsureIsDetailType(pipeline, DetailType::Vulkan);
				vk_pipeline = *vulkan_pipeline;
				break;
			}
			};

			if (vk_pipeline)
			{
				vkCmdBindPipeline(*command_buffer, VulkanPipelineUsage{usage}.GetVkPipelineBindPoint(), vk_pipeline);
				applied = true;
			}
			return applied;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanBindIndexBufferCommand : public BindIndexBufferCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			bl applied = false;
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			if (buffer)
			{
				VkIndexType index_type = VK_INDEX_TYPE_MAX_ENUM;

				switch (stride)
				{
				case (sizeof(ui16)):
					index_type = VK_INDEX_TYPE_UINT16;
					break;
				case (sizeof(ui32)):
					index_type = VK_INDEX_TYPE_UINT32;
					break;
				};

				vkCmdBindIndexBuffer(*command_buffer, *buffer, offset, index_type);
				applied = true;
			}
			return applied;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanBindVertexBuffersCommand : public BindVertexBuffersCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			con::vector<VkBuffer> buffers(contexts.size());
			con::vector<VkDeviceSize> offsets(contexts.size());

			for (siz i = 0; i < contexts.size(); i++)
			{
				mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->contexts[i].buffer, DetailType::Vulkan);
				buffers[i] = buffer ? *buffer : (VkBuffer)nullptr;
				offsets[i] = contexts[i].offset;
			}

			vkCmdBindVertexBuffers(*command_buffer, 0, buffers.size(), buffers.empty() ? nullptr : buffers.data(), offsets.data());
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanBindResourceGroupCommand : public BindResourceGroupCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			//TODO: implement vkCmdBindDescriptorSets
			//vkCmdBindDescriptorSets(command_buffer, pipelineBindPoint, pipelineLayout, firstSet, (ui32)descriptorSets.size(), descriptorSets.data(), (ui32)dynamicOffsets.size(), dynamicOffsets.data());
			return false;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	struct VulkanBarrier
	{
		VulkanAccess dstAccess = VulkanAccess::None;
		VulkanAccess srcAccess = VulkanAccess::None;

		VulkanBarrier(const Barrier& other = {}) :dstAccess(other.dstAccess), srcAccess(other.srcAccess) {}

		operator Barrier() const
		{
			return { dstAccess, srcAccess };
		}

		VkMemoryBarrier GetVkMemoryBarrier() const
		{
			VkMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
			barrier.dstAccessMask = dstAccess.GetVkAccessFlags();
			barrier.srcAccessMask = srcAccess.GetVkAccessFlags();
			return barrier;
		}
	};

	struct VulkanBufferBarrier : public VulkanBarrier
	{
		mem::sptr<VulkanBufferResource> buffer = nullptr;
		ui32 offset = 0;
		ui32 size = 0; //SIZ_MAX equates to the remaining portion of the buffer after offset (aka, VK_WHOLE_SIZE)
		DeviceQueueFamily dstQueueFamily{};
		DeviceQueueFamily srcQueueFamily{};

		VulkanBufferBarrier(const BufferBarrier& other = {}) :
			VulkanBarrier(other),
			buffer(other.buffer),
			offset(other.offset),
			size(other.size),
			dstQueueFamily(other.dstQueueFamily),
			srcQueueFamily(other.srcQueueFamily)
		{}

		operator BufferBarrier() const
		{
			return { dstAccess, srcAccess, buffer, offset, size, dstQueueFamily, srcQueueFamily };
		}

		VkBufferMemoryBarrier GetVkBufferMemoryBarrier() const
		{
			VkBufferMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
			barrier.srcAccessMask = srcAccess.GetVkAccessFlags();
			barrier.dstAccessMask = dstAccess.GetVkAccessFlags();
			barrier.srcQueueFamilyIndex = srcQueueFamily.index;
			barrier.dstQueueFamilyIndex = dstQueueFamily.index;
			barrier.buffer = *buffer;
			barrier.offset = offset;
			barrier.size = size;
			return barrier;
		}
	};

	struct VulkanImageBarrier : public VulkanBarrier
	{
		mem::sptr<VulkanImageResource> image = nullptr;
		//TODO: add image layout fields?
		//TODO: add subresource range field?
		DeviceQueueFamily dstQueueFamily{};
		DeviceQueueFamily srcQueueFamily{};

		VulkanImageBarrier(const ImageBarrier& other):
			image(other.image),
			dstQueueFamily(other.dstQueueFamily),
			srcQueueFamily(other.srcQueueFamily)
		{}

		operator ImageBarrier() const
		{
			return { dstAccess, srcAccess, image, dstQueueFamily, srcQueueFamily };
		}

		VkImageMemoryBarrier GetVkImageMemoryBarrier() const
		{
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.srcAccessMask = srcAccess.GetVkAccessFlags();
			barrier.dstAccessMask = dstAccess.GetVkAccessFlags();
			//barrier.oldLayout = 0;// TODO: determine this
			//barrier.newLayout = 0; //TODO: determine this
			barrier.srcQueueFamilyIndex = srcQueueFamily.index;
			barrier.dstQueueFamilyIndex = dstQueueFamily.index;
			barrier.image = *image;
			//barrier.subresourceRange = 0; //TODO: determine this
			return barrier;
		}
	};

	class VulkanBarrierCommand : public BarrierCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			/*
				
				TODO: note that image layout changes are considered a color write access
			
			*/
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			const VulkanStage dstStage = this->dstStage;
			const VulkanStage srcStage = this->srcStage;

			con::vector<VkMemoryBarrier> vk_barriers(barriers.size());
			for (siz i = 0; i < vk_barriers.size(); i++)
				vk_barriers[i] = VulkanBarrier{ barriers[i] }.GetVkMemoryBarrier();

			con::vector<VkBufferMemoryBarrier> vk_buffer_barriers(bufferBarriers.size());
			for (siz i = 0; i < vk_buffer_barriers.size(); i++)
				vk_buffer_barriers[i] = VulkanBufferBarrier{ bufferBarriers[i] }.GetVkBufferMemoryBarrier();

			con::vector<VkImageMemoryBarrier> vk_image_barriers(imageBarriers.size());
			for (siz i = 0; i < vk_image_barriers.size(); i++)
				vk_image_barriers[i] = VulkanImageBarrier{ imageBarriers[i] }.GetVkImageMemoryBarrier();

			vkCmdPipelineBarrier(*command_buffer,
				srcStage.GetVkPipelineStageFlags(),
				dstStage.GetVkPipelineStageFlags(),
				0, //TODO: do we want to do anything with this?
				vk_barriers.size(), vk_barriers.empty() ? nullptr : vk_barriers.data(),
				vk_buffer_barriers.size(), vk_buffer_barriers.empty() ? nullptr : vk_buffer_barriers.data(),
				vk_image_barriers.size(), vk_image_barriers.empty() ? nullptr : vk_image_barriers.data());
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanExecuteCommandsCommand : public ExecuteCommandsCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			con::vector<VkCommandBuffer> command_buffers(commandBuffers.size());
			for (siz i = 0; i < command_buffers.size(); i++)
			{
				mem::sptr<VulkanCommandBuffer> vulkan_command_buffer = EnsureIsDetailType(commandBuffers[i], DetailType::Vulkan);
				command_buffers[i] = *vulkan_command_buffer;
			}

			vkCmdExecuteCommands(*command_buffer, command_buffers.size(), command_buffers.empty() ? nullptr : command_buffers.data());
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanPushDataCommand : public PushDataCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			bl applied = false;
			mem::sptr<VulkanPipelineResourceLayout> layout = EnsureIsDetailType(pipeline->GetPipelineResourceLayout(), DetailType::Vulkan);

			if (layout)
			{
				const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
				VulkanStage stage = this->stage;
				con::vector<ui8> bytes = layout->GetPushData().GetAllEntryBytes();
				vkCmdPushConstants(*command_buffer, *layout, stage.GetVkShaderStageFlags(), 0, bytes.size(), bytes.empty() ? nullptr : bytes.data());
				applied = true;
			}

			return applied;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};


	class VulkanSetViewportsCommand : public SetViewportsCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			con::vector<VkViewport> viewports(this->viewports.size());
			for (siz i = 0; i < viewports.size(); i++)
				viewports[i] = VulkanViewport{ this->viewports[i] }.GetVkViewport();

			vkCmdSetViewport(*command_buffer, 0, viewports.size(), viewports.empty() ? nullptr : viewports.data());
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanSetScissorsCommand : public SetScissorsCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			con::vector<VkRect2D> scissors(this->scissors.size());
			for (siz i = 0; i < scissors.size(); i++)
				scissors[i] = VulkanScissor{ this->scissors[i] }.GetVkRect2D();

			vkCmdSetScissor(*command_buffer, 0, scissors.size(), scissors.empty() ? nullptr : scissors.data());
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanSetDepthBoundsCommand : public SetDepthBoundsCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			vkCmdSetDepthBounds(*command_buffer, depthBounds.min, depthBounds.max);
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanDispatchCommand : public DispatchCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			vkCmdDispatch(*command_buffer, x, y, z);
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanIndirectDispatchCommand : public IndirectDispatchCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			bl applied = false;
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			if (buffer)
			{
				vkCmdDispatchIndirect(*command_buffer, *buffer, offset);
				applied = true;
			}
			return applied;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanDrawCommand : public DrawCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			vkCmdDraw(*command_buffer, vertexCount, instanceCount, vertexBeginIndex, instanceBeginIndex);
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	class VulkanDrawIndexedCommand : public DrawIndexedCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			vkCmdDrawIndexed(*command_buffer, indexCount, instanceCount, indexBeginIndex, vertexOffset, instanceBeginIndex);
			return true;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual bl IsPrepared() const override
		{
			return true;
		}
	};

	struct VulkanDrawIndirectCommandPayload
	{
		ui32 vertexCount = 0;
		ui32 vertexBeginIndex = 0;
		ui32 instanceCount = 0; //TODO: does this have to default to 1? investigate
		ui32 instanceBeginIndex = 0;

		VulkanDrawIndirectCommandPayload(const DrawIndirectCommandPayload& other):
			vertexCount(other.vertexCount),
			vertexBeginIndex(other.vertexBeginIndex),
			instanceCount(other.instanceCount),
			instanceBeginIndex(other.instanceBeginIndex)
		{}

		operator DrawIndirectCommandPayload() const
		{
			return { vertexCount, vertexBeginIndex, instanceCount, instanceBeginIndex };
		}

		VkDrawIndirectCommand GetVkDrawIndirectCommand() const
		{
			return { vertexCount, instanceCount, vertexBeginIndex, instanceBeginIndex };
		}
	};

	class VulkanDrawIndirectCommand : public DrawIndirectCommand
	{
	protected:
		bl prepared = false;

		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			bl applied = false;
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			if (buffer) //TODO: I'm assuming this is all correct -- investigate
			{
				vkCmdDrawIndirect(*command_buffer, *buffer, payloadOffset, drawCount, sizeof(VkDrawIndirectCommand));
				applied = true;
			}
			return applied;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		/*
			required to call this before adding to a command buffer
			this methods sets payloadOffset and payloadCount
		*/
		virtual bl Prepare(siz offset, const con::vector<DrawIndirectCommandPayload>& payloads_) override
		{
			//TODO: I'm assuming this is all correct -- investigate

			prepared = false;
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);

			if (buffer)
			{
				con::vector<VkDrawIndirectCommand> payloads(payloads_.size());
				for (siz i = 0; i < payloads.size(); i++)
					payloads[i] = VulkanDrawIndirectCommandPayload{ payloads_[i] }.GetVkDrawIndirectCommand();

				con::vector<ui8> bytes(payloads.size() * sizeof(VkDrawIndirectCommand));
				mem::CopyBytes(bytes.data(), payloads.data(), bytes.size());
				prepared = buffer->Assign(offset, bytes);

				if (prepared)
				{
					payloadOffset = offset;
					payloadCount = payloads.size();
				}
			}

			return prepared;
		}

		virtual bl IsPrepared() const override
		{
			return prepared;
		}
	};

	struct VulkanDrawIndexedIndirectCommandPayload
	{
		ui32 indexCount = 0;
		ui32 indexBeginIndex = 0;
		i32 vertexOffset = 0;
		ui32 instanceCount = 0;
		ui32 instanceBeginIndex = 0;

		VulkanDrawIndexedIndirectCommandPayload(const DrawIndexedIndirectCommandPayload& other) :
			indexCount(other.indexCount),
			indexBeginIndex(other.indexBeginIndex),
			vertexOffset(other.vertexOffset),
			instanceCount(other.instanceCount),
			instanceBeginIndex(other.instanceBeginIndex)
		{}

		operator DrawIndexedIndirectCommandPayload() const
		{
			return { indexCount, indexBeginIndex, (siz)vertexOffset, instanceCount, instanceBeginIndex };
		}

		VkDrawIndexedIndirectCommand GetVkDrawIndexedIndirectCommand() const
		{
			return { indexCount, instanceCount, indexBeginIndex, vertexOffset, instanceBeginIndex };
		}
	};

	class VulkanDrawIndexedIndirectCommand : public DrawIndexedIndirectCommand
	{
	protected:
		bl prepared = false;

		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			bl applied = false;
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			if (buffer)
			{
				vkCmdDrawIndexedIndirect(*command_buffer, *buffer, payloadOffset, drawCount, sizeof(VkDrawIndexedIndirectCommand));
				applied = true;
			}
			return applied;
		}

	public:
		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		/*
			required to call this before adding to a command buffer
			this methods sets payloadOffset and payloadCount
		*/
		virtual bl Prepare(siz offset, const con::vector<DrawIndexedIndirectCommandPayload>& payloads_) override
		{
			//TODO: I'm assuming this is all correct -- investigate

			prepared = false;
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);

			if (buffer)
			{
				con::vector<VkDrawIndexedIndirectCommand> payloads(payloads_.size());
				for (siz i = 0; i < payloads.size(); i++)
					payloads[i] = VulkanDrawIndexedIndirectCommandPayload{ payloads_[i] }.GetVkDrawIndexedIndirectCommand();

				con::vector<ui8> bytes(payloads.size() * sizeof(VkDrawIndexedIndirectCommand));
				mem::CopyBytes(bytes.data(), payloads.data(), bytes.size());
				prepared = buffer->Assign(offset, bytes);

				if (prepared)
				{
					payloadOffset = offset;
					payloadCount = payloads.size();
				}
			}

			return prepared;
		}

		virtual bl IsPrepared() const override
		{
			return prepared;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_COMMANDS_HPP */