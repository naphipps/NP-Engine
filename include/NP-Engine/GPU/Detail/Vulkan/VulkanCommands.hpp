//##===----------------------------------------------------------------------===##//
//
// Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_COMMANDS_HPP
#define NP_ENGINE_GPU_VULKAN_COMMANDS_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Commands.hpp"

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
#include "VulkanResourceGroup.hpp"

namespace np::gpu::__detail
{
	struct VulkanCommand;

	struct VulkanCopyBufferRange
	{
		siz dstOffset;
		siz srcOffset;
		siz size;

		VulkanCopyBufferRange(const CopyBufferRange& other = {}):
			dstOffset(other.dstOffset),
			srcOffset(other.srcOffset),
			size(other.size)
		{}

		operator CopyBufferRange() const
		{
			return {dstOffset, srcOffset, size};
		}

		VkBufferCopy GetVkBufferCopy() const
		{
			return {srcOffset, dstOffset, size};
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

			bl applied = false;
			if (src && dst)
			{
				con::vector<VkBufferCopy> ranges(this->ranges.size());
				for (siz i = 0; i < ranges.size(); i++)
					ranges[i] = VulkanCopyBufferRange{ this->ranges[i] }.GetVkBufferCopy();

				vkCmdCopyBuffer(*command_buffer, *src, *dst, ranges.size(), ranges.empty() ? nullptr : ranges.data());
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

	struct VulkanImageLayers
	{
		VulkanImageResourceUsage usage;
		ui32 mipLevel;
		ui32 layerCount;
		ui32 layerBeginIndex;

		VulkanImageLayers(const ImageLayers& other = {}) :
			usage(other.usage),
			mipLevel(other.mipLevel),
			layerCount(other.layerCount),
			layerBeginIndex(other.layerBeginIndex)
		{}

		operator ImageLayers() const
		{
			return { usage, mipLevel, layerCount, layerBeginIndex };
		}

		VkImageSubresourceLayers GetVkImageSubresourceLayers() const
		{
			return { usage.GetVkAspectFlags(), mipLevel, layerBeginIndex, layerCount};
		}
	};

	struct VulkanImageRange
	{
		VulkanImageResourceUsage usage;
		ui32 mipCount;
		ui32 mipBeginIndex;
		ui32 layerCount;
		ui32 layerBeginIndex;

		VulkanImageRange(const ImageRange& other = {}):
			usage(other.usage),
			mipCount(other.mipCount),
			mipBeginIndex(other.mipBeginIndex),
			layerCount(other.layerCount),
			layerBeginIndex(other.layerBeginIndex)
		{}

		operator ImageRange() const
		{
			return { usage, mipCount, mipBeginIndex, layerCount, layerBeginIndex };
		}

		VkImageSubresourceRange GetVkImageSubresourceRange() const
		{
			return { usage.GetVkAspectFlags(), mipBeginIndex, mipCount, layerBeginIndex, layerCount };
		}
	};

	struct VulkanCopyImageContext
	{
		VkOffset3D dstOffset;
		VkOffset3D srcOffset;
		VkExtent3D size;
		VulkanImageLayers dstLayers;
		VulkanImageLayers srcLayers;

		VulkanCopyImageContext(const CopyImageContext& other = {}) :
			dstOffset(other.dstOffset.x, other.dstOffset.y, other.dstOffset.z),
			srcOffset(other.srcOffset.x, other.srcOffset.y, other.srcOffset.z),
			size(other.size.x, other.size.y, other.size.z),
			dstLayers(other.dstLayers),
			srcLayers(other.srcLayers)
		{}

		operator CopyImageContext() const
		{
			return
			{ 
				{dstOffset.x, dstOffset.y, dstOffset.z},
				{srcOffset.x, srcOffset.y, srcOffset.z},
				{size.width, size.height, size.depth},
				dstLayers, srcLayers
			};
		}

		VkImageCopy GetVkImageCopy() const
		{
			return
			{ 
				srcLayers.GetVkImageSubresourceLayers(), srcOffset,
				dstLayers.GetVkImageSubresourceLayers(), dstOffset,
				size
			};
		}
	};

	class VulkanCopyImageCommand : public CopyImageCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			mem::sptr<VulkanImageResource> dst = EnsureIsDetailType(this->dst, DetailType::Vulkan);
			mem::sptr<VulkanImageResource> src = EnsureIsDetailType(this->src, DetailType::Vulkan);

			bl applied = false;
			if (dst && src)
			{
				const VulkanImageResourceUsage dst_usage{ dstUsage };
				const VulkanImageResourceUsage src_usage{ srcUsage };

				con::vector<VkImageCopy> contexts(this->contexts.size());
				for (siz i = 0; i < contexts.size(); i++)
					contexts[i] = VulkanCopyImageContext{ this->contexts[i] }.GetVkImageCopy();
				
				vkCmdCopyImage(*command_buffer, *src, src_usage.GetVkImageLayout(),
					*dst, dst_usage.GetVkImageLayout(),
					contexts.size(), contexts.empty() ? nullptr : contexts.data());
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

	struct VulkanCopyBufferToImageRange
	{
		VkOffset3D imageOffset;
		VkExtent3D imageSize;
		VulkanImageLayers imageLayers;
		siz bufferOffset;
		ui32 bufferRowCount;
		ui32 bufferRowLength;

		VulkanCopyBufferToImageRange(const CopyBufferToImageRange& other = {}):
			imageOffset(other.imageOffset.x, other.imageOffset.y, other.imageOffset.z),
			imageSize(other.imageSize.x, other.imageSize.y, other.imageSize.z),
			imageLayers(other.imageLayers),
			bufferOffset(other.bufferOffset),
			bufferRowCount(other.bufferRowCount),
			bufferRowLength(other.bufferRowLength)
		{}

		operator CopyBufferToImageRange() const
		{
			return
			{
				{imageOffset.x,imageOffset.y, imageOffset.z},
				{imageSize.width, imageSize.height, imageSize.depth},
				imageLayers, bufferOffset, bufferRowCount, bufferRowLength
			};
		}

		/*
			NOTE: caller must set aspectMask for imageSubresource
		*/
		VkBufferImageCopy GetVkBufferImageCopy() const
		{
			return
			{
				bufferOffset, bufferRowLength, bufferRowCount,
				imageLayers.GetVkImageSubresourceLayers(), imageOffset, imageSize
			};
		}
	};

	class VulkanCopyBufferToImageCommand : public CopyBufferToImageCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			mem::sptr<VulkanImageResource> image = EnsureIsDetailType(this->image, DetailType::Vulkan);
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);

			bl applied = false;
			if (image && buffer)
			{
				const VulkanImageResourceUsage image_usage{ imageUsage };

				con::vector<VkBufferImageCopy> ranges(this->ranges.size());
				for (siz i = 0; i < ranges.size(); i++)
					ranges[i] = VulkanCopyBufferToImageRange{ this->ranges[i] }.GetVkBufferImageCopy();

				vkCmdCopyBufferToImage(*command_buffer, *buffer, *image, image_usage.GetVkImageLayout(),
					ranges.size(), ranges.empty() ? nullptr : ranges.data());
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

	using VulkanCopyImageToBufferRange = VulkanCopyBufferToImageRange;

	class VulkanCopyImageToBufferCommand : public CopyImageToBufferCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			mem::sptr<VulkanImageResource> image = EnsureIsDetailType(this->image, DetailType::Vulkan);
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);

			bl applied = false;
			if (buffer && image)
			{
				const VulkanImageResourceUsage image_usage{ imageUsage };

				con::vector<VkBufferImageCopy> ranges(this->ranges.size());
				for (siz i = 0; i < ranges.size(); i++)
				{
					ranges[i] = VulkanCopyImageToBufferRange{ this->ranges[i] }.GetVkBufferImageCopy();
					ranges[i].imageSubresource.aspectMask = image_usage.GetVkAspectFlags();
				}

				vkCmdCopyImageToBuffer(*command_buffer, *image, image_usage.GetVkImageLayout(), *buffer, 
					ranges.size(), ranges.empty() ? nullptr : ranges.data());
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

	class VulkanFillBufferCommand : public FillBufferCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			bl applied = true;
			if (buffer && offset < buffer->GetSize() && offset + count <= buffer->GetSize())
			{
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
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			bl applied = true;
			if (buffer && offset < buffer->GetSize() && offset + values.size() <= buffer->GetSize())
			{
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

	struct VulkanRenderArea
	{
		::glm::i32vec2 offset;
		ui32 width;
		ui32 height;

		VulkanRenderArea(const RenderArea& other = {}): offset(other.offset), width(other.width), height(other.height) {}

		operator RenderArea() const
		{
			return {offset, width, height};
		}

		VkRect2D GetVkRect2D() const
		{
			return {{offset.x, offset.y}, {width, height}};
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
				con::vector<mem::sptr<ImageResourceView>> views = framebuffer->GetImageResourceViews();
				if (views.size() == clearColors.size())
				{
					bl is_valid = true;
					con::vector<VkClearValue> clear_values(clearColors.size());
					for (siz i = 0; is_valid && i < clear_values.size(); i++)
					{
						mem::sptr<VulkanImageResourceView> view = EnsureIsDetailType(views[i], DetailType::Vulkan);
						mem::sptr<VulkanImageResource> image = EnsureIsDetailType(view->GetImageResource(), DetailType::Vulkan);
						is_valid &= view && image;
						if (is_valid)
						{
							const VulkanClearColor clear_color{ clearColors[i] };
							const VulkanFormat format{ image->GetFormat() };

							if (format.ContainsAny(VulkanImageResourceUsage::Depth | VulkanImageResourceUsage::Stencil))
								clear_values[i].depthStencil = clear_color.GetVkClearDepthStencilValue();
							else
								clear_values[i].color = clear_color.GetVkClearColorValue();
						}
					}

					if (is_valid)
					{
						const VulkanSubpassUsage usage{ this->usage };
						const VulkanRenderArea render_area = renderArea;

						VkRenderPassBeginInfo info = GetVkRenderPassBeginInfo(framebuffer, render_area.GetVkRect2D());
						info.clearValueCount = clear_values.size();
						info.pClearValues = clear_values.empty() ? nullptr : clear_values.data();

						vkCmdBeginRenderPass(*command_buffer, &info, usage.GetVkSubpassContents()); //TODO: implement
						applied = true;
					}
				}
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
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			VulkanSubpassUsage usage{ this->usage };
			vkCmdNextSubpass(*command_buffer, usage.GetVkSubpassContents());
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
				mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(contexts[i].buffer, DetailType::Vulkan);
				buffers[i] = buffer ? *buffer : static_cast<VkBuffer>(nullptr);
				offsets[i] = contexts[i].offset;
			}

			vkCmdBindVertexBuffers(*command_buffer, 0, buffers.size(), buffers.empty() ? nullptr : buffers.data(),
								   offsets.data());
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

	class VulkanBindResourceGroupsCommand : public BindResourceGroupsCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			bl applied = false;

			if (pipeline && EnsureIsDetailType(pipeline, DetailType::Vulkan) != nullptr)
			{
				VulkanPipelineUsage pipeline_usage = VulkanPipelineUsage::None;
				switch (pipeline->GetPipelineType())
				{
				case PipelineType::Graphics:
					pipeline_usage = VulkanPipelineUsage::Graphics;
					break;

				case PipelineType::Compute:
					pipeline_usage = VulkanPipelineUsage::Compute;
					break;
				}

				if (!pipeline_usage.Equals(VulkanPipelineUsage::None))
				{
					mem::sptr<VulkanPipelineResourceLayout> layout = EnsureIsDetailType(pipeline->GetPipelineResourceLayout(), DetailType::Vulkan);

					if (layout)
					{
						const con::vector<mem::sptr<ResourceLayout>> resource_layouts = layout->GetResourceLayouts();
						bl is_compatible = resource_layouts.size() >= resourceLayoutBeginIndex + resourceGroups.size();

						con::vector<VkDescriptorSet> sets(resourceGroups.size());
						for (siz i = 0; is_compatible && i < sets.size(); i++)
						{
							mem::sptr<VulkanResourceGroup> resource_group = resourceGroups[i];
							mem::sptr<ResourceLayout> resource_layout = resource_layouts[resourceLayoutBeginIndex + i];
							is_compatible &= resource_group->IsCompatible(resource_layout);
							sets[i] = *resource_group;
						}

						if (is_compatible)
						{
							con::vector<ui32> offsets(dynamicResourceOffsets.size());
							for (siz i = 0; i < offsets.size(); i++)
								offsets[i] = static_cast<ui32>(dynamicResourceOffsets[i]);

							/*
								TODO: check if offsets are compatible here?
									- I know/am-pretty-sure offsets.size() must equal sets.size()
							*/

							vkCmdBindDescriptorSets(*command_buffer, pipeline_usage.GetVkPipelineBindPoint(), *layout, 0,
								sets.size(), sets.empty() ? nullptr : sets.data(),
								offsets.size(), offsets.empty() ? nullptr : offsets.data());
							applied = true;
						}
					}
				}
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

	struct VulkanBarrier
	{
		VulkanAccess dstAccess = VulkanAccess::None;
		VulkanAccess srcAccess = VulkanAccess::None;

		VulkanBarrier(const Barrier& other = {}): dstAccess(other.dstAccess), srcAccess(other.srcAccess) {}

		operator Barrier() const
		{
			return {dstAccess, srcAccess};
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
		mem::sptr<VulkanBufferResource> buffer;
		siz offset;
		siz size;
		DeviceQueueFamily dstQueueFamily;
		DeviceQueueFamily srcQueueFamily;

		VulkanBufferBarrier(const BufferBarrier& other = {}):
			VulkanBarrier(other),
			buffer(other.buffer),
			offset(other.offset),
			size(other.size),
			dstQueueFamily(other.dstQueueFamily),
			srcQueueFamily(other.srcQueueFamily)
		{}

		operator BufferBarrier() const
		{
			return {dstAccess, srcAccess, buffer, offset, size, dstQueueFamily, srcQueueFamily};
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
		mem::sptr<VulkanImageResource> image;
		VulkanImageResourceUsage dstImageResourceUsage;
		VulkanImageResourceUsage srcImageResourceUsage;
		DeviceQueueFamily dstQueueFamily;
		DeviceQueueFamily srcQueueFamily;
		VulkanImageRange range;

		VulkanImageBarrier(const ImageBarrier& other = {}) :
			image(other.image),
			dstImageResourceUsage(other.dstImageResourceUsage),
			srcImageResourceUsage(other.srcImageResourceUsage),
			dstQueueFamily(other.dstQueueFamily),
			srcQueueFamily(other.srcQueueFamily),
			range(other.range)
		{}

		operator ImageBarrier() const
		{
			return {dstAccess, srcAccess, image, dstImageResourceUsage, srcImageResourceUsage, dstQueueFamily, srcQueueFamily, range };
		}

		VkImageMemoryBarrier GetVkImageMemoryBarrier() const
		{
			VkImageMemoryBarrier barrier{};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.srcAccessMask = srcAccess.GetVkAccessFlags();
			barrier.dstAccessMask = dstAccess.GetVkAccessFlags();
			barrier.oldLayout = srcImageResourceUsage.GetVkImageLayout();
			barrier.newLayout = dstImageResourceUsage.GetVkImageLayout();
			barrier.srcQueueFamilyIndex = srcQueueFamily.index;
			barrier.dstQueueFamilyIndex = dstQueueFamily.index;
			barrier.image = *image;
			barrier.subresourceRange = range.GetVkImageSubresourceRange();
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
			const VulkanStage dst_stage{dstStage};
			const VulkanStage src_stage{srcStage};

			con::vector<VkMemoryBarrier> vk_barriers(barriers.size());
			for (siz i = 0; i < vk_barriers.size(); i++)
				vk_barriers[i] = VulkanBarrier{barriers[i]}.GetVkMemoryBarrier();

			con::vector<VkBufferMemoryBarrier> vk_buffer_barriers(bufferBarriers.size());
			for (siz i = 0; i < vk_buffer_barriers.size(); i++)
				vk_buffer_barriers[i] = VulkanBufferBarrier{bufferBarriers[i]}.GetVkBufferMemoryBarrier();

			con::vector<VkImageMemoryBarrier> vk_image_barriers(imageBarriers.size());
			for (siz i = 0; i < vk_image_barriers.size(); i++)
				vk_image_barriers[i] = VulkanImageBarrier{imageBarriers[i]}.GetVkImageMemoryBarrier();

			vkCmdPipelineBarrier(*command_buffer, src_stage.GetVkPipelineStageFlags(), dst_stage.GetVkPipelineStageFlags(),
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
				mem::sptr<VulkanCommandBuffer> vulkan_command_buffer =
					EnsureIsDetailType(commandBuffers[i], DetailType::Vulkan);
				command_buffers[i] = *vulkan_command_buffer;
			}

			vkCmdExecuteCommands(*command_buffer, command_buffers.size(),
								 command_buffers.empty() ? nullptr : command_buffers.data());
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
			mem::sptr<VulkanPipelineResourceLayout> layout =
				EnsureIsDetailType(pipeline->GetPipelineResourceLayout(), DetailType::Vulkan);

			if (layout)
			{
				const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
				const VulkanStage stage{this->stage};
				con::vector<ui8> bytes = layout->GetPushData().GetAllEntryBytes();
				vkCmdPushConstants(*command_buffer, *layout, stage.GetVkShaderStageFlags(), 0, bytes.size(),
								   bytes.empty() ? nullptr : bytes.data());
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
				viewports[i] = VulkanViewport{this->viewports[i]}.GetVkViewport();

			vkCmdSetViewport(*command_buffer, viewportBeginIndex, viewports.size(), viewports.empty() ? nullptr : viewports.data());
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
				scissors[i] = VulkanScissor{this->scissors[i]}.GetVkRect2D();

			vkCmdSetScissor(*command_buffer, scissorBeginIndex, scissors.size(), scissors.empty() ? nullptr : scissors.data());
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
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			bl applied = false;
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

	class VulkanWaitFlagsCommand : public WaitFlagsCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			/*
				TODO: note that image layout changes are considered a color write access
			*/
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			const VulkanStage dst_stage{ dstStage };
			const VulkanStage src_stage{ srcStage };
			bl applied = false;

			bl found_nullptr = false;
			con::vector<VkEvent> vk_flags(flags.size());
			for (siz i = 0; !found_nullptr && i < vk_flags.size(); i++)
			{
				mem::sptr<VulkanFlag> flag = EnsureIsDetailType(flags[i], DetailType::Vulkan);
				vk_flags[i] = flag ? *flag : nullptr;
				found_nullptr |= vk_flags[i] == nullptr;
			}

			if (found_nullptr)
			{
				con::vector<VkMemoryBarrier> vk_barriers(barriers.size());
				for (siz i = 0; i < vk_barriers.size(); i++)
					vk_barriers[i] = VulkanBarrier{ barriers[i] }.GetVkMemoryBarrier();

				con::vector<VkBufferMemoryBarrier> vk_buffer_barriers(bufferBarriers.size());
				for (siz i = 0; i < vk_buffer_barriers.size(); i++)
					vk_buffer_barriers[i] = VulkanBufferBarrier{ bufferBarriers[i] }.GetVkBufferMemoryBarrier();

				con::vector<VkImageMemoryBarrier> vk_image_barriers(imageBarriers.size());
				for (siz i = 0; i < vk_image_barriers.size(); i++)
					vk_image_barriers[i] = VulkanImageBarrier{ imageBarriers[i] }.GetVkImageMemoryBarrier();

				vkCmdWaitEvents(*command_buffer, vk_flags.size(), vk_flags.empty() ? nullptr : vk_flags.data(),
					src_stage.GetVkPipelineStageFlags(), dst_stage.GetVkPipelineStageFlags(),
					vk_barriers.size(), vk_barriers.empty() ? nullptr : vk_barriers.data(),
					vk_buffer_barriers.size(), vk_buffer_barriers.empty() ? nullptr : vk_buffer_barriers.data(),
					vk_image_barriers.size(), vk_image_barriers.empty() ? nullptr : vk_image_barriers.data());
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

	class VulkanSetFlagCommand : public SetFlagCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			mem::sptr<VulkanFlag> flag = EnsureIsDetailType(this->flag, DetailType::Vulkan);
			bl applied = false;
			if (flag)
			{
				VulkanStage stage{ this->stage };
				vkCmdSetEvent(*command_buffer, *flag, stage.GetVkPipelineStageFlags());
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

	class VulkanResetFlagCommand : public ResetFlagCommand
	{
	protected:
		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			mem::sptr<VulkanFlag> flag = EnsureIsDetailType(this->flag, DetailType::Vulkan);
			bl applied = false;
			if (flag)
			{
				VulkanStage stage{ this->stage };
				vkCmdResetEvent(*command_buffer, *flag, stage.GetVkPipelineStageFlags());
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
		ui32 vertexCount;
		ui32 vertexBeginIndex;
		ui32 instanceCount;
		ui32 instanceBeginIndex;

		VulkanDrawIndirectCommandPayload(const DrawIndirectCommandPayload& other = {}) :
			vertexCount(other.vertexCount),
			vertexBeginIndex(other.vertexBeginIndex),
			instanceCount(other.instanceCount),
			instanceBeginIndex(other.instanceBeginIndex)
		{}

		operator DrawIndirectCommandPayload() const
		{
			return {vertexCount, vertexBeginIndex, instanceCount, instanceBeginIndex};
		}

		VkDrawIndirectCommand GetVkDrawIndirectCommand() const
		{
			return {vertexCount, instanceCount, vertexBeginIndex, instanceBeginIndex};
		}
	};

	class VulkanDrawIndirectCommand : public DrawIndirectCommand
	{
	protected:
		bl prepared = false;

		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			bl applied = false;
			if (buffer)
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
			this method DOES NOT call buffer's ClearCacheForDevice
		*/
		virtual bl Prepare(siz offset, const con::vector<DrawIndirectCommandPayload>& payloads_) override
		{
			//TODO: I'm assuming this is all correct -- investigate
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			prepared = false;
			
			if (buffer)
			{
				con::vector<VkDrawIndirectCommand> payloads(payloads_.size());
				for (siz i = 0; i < payloads.size(); i++)
					payloads[i] = VulkanDrawIndirectCommandPayload{payloads_[i]}.GetVkDrawIndirectCommand();

				con::vector<ui8> bytes(payloads.size() * sizeof(VkDrawIndirectCommand));
				mem::copy_bytes(bytes.data(), payloads.data(), bytes.size());
				prepared = buffer->SetBytes(offset, bytes);
				
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
		ui32 indexCount;
		ui32 indexBeginIndex;
		i32 vertexOffset;
		ui32 instanceCount;
		ui32 instanceBeginIndex;

		VulkanDrawIndexedIndirectCommandPayload(const DrawIndexedIndirectCommandPayload& other = {}) :
			indexCount(other.indexCount),
			indexBeginIndex(other.indexBeginIndex),
			vertexOffset(other.vertexOffset),
			instanceCount(other.instanceCount),
			instanceBeginIndex(other.instanceBeginIndex)
		{}

		operator DrawIndexedIndirectCommandPayload() const
		{
			return {indexCount, indexBeginIndex, (siz)vertexOffset, instanceCount, instanceBeginIndex};
		}

		VkDrawIndexedIndirectCommand GetVkDrawIndexedIndirectCommand() const
		{
			return {indexCount, instanceCount, indexBeginIndex, vertexOffset, instanceBeginIndex};
		}
	};

	class VulkanDrawIndexedIndirectCommand : public DrawIndexedIndirectCommand
	{
	protected:
		bl prepared = false;

		virtual bl ApplyTo(const CommandBuffer* command_buffer_) override
		{
			const VulkanCommandBuffer* command_buffer = static_cast<const VulkanCommandBuffer*>(command_buffer_);
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			bl applied = false;
			if (buffer)
			{
				vkCmdDrawIndexedIndirect(*command_buffer, *buffer, payloadOffset, drawCount,
										 sizeof(VkDrawIndexedIndirectCommand));
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
			this method DOES NOT call buffer's ClearCacheForDevice
		*/
		virtual bl Prepare(siz offset, const con::vector<DrawIndexedIndirectCommandPayload>& payloads_) override
		{
			//TODO: I'm assuming this is all correct -- investigate
			mem::sptr<VulkanBufferResource> buffer = EnsureIsDetailType(this->buffer, DetailType::Vulkan);
			prepared = false;

			if (buffer)
			{
				con::vector<VkDrawIndexedIndirectCommand> payloads(payloads_.size());
				for (siz i = 0; i < payloads.size(); i++)
					payloads[i] = VulkanDrawIndexedIndirectCommandPayload{payloads_[i]}.GetVkDrawIndexedIndirectCommand();

				con::vector<ui8> bytes(payloads.size() * sizeof(VkDrawIndexedIndirectCommand));
				mem::copy_bytes(bytes.data(), payloads.data(), bytes.size());
				prepared = buffer->SetBytes(offset, bytes);

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