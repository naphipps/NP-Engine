//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_PASS_HPP
#define NP_ENGINE_VULKAN_RENDER_PASS_HPP

#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanCommands.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanDevice.hpp"
#include "VulkanTexture.hpp"
#include "VulkanFrame.hpp"

namespace np::graphics::rhi
{
	class VulkanRenderPass
	{
	private:
		VulkanDevice& _device;
		VkRenderPass _render_pass;
		VulkanTexture* _depth_texture;
		VulkanCommandBeginRenderPass* _begin_render_pass;
		VulkanCommandEndRenderPass _end_render_pass;

		VkAttachmentDescription CreateColorAttachmentDescription() const
		{
			VkAttachmentDescription desc{};
			desc.format = GetDevice().GetSurfaceFormat().format;
			desc.samples = VK_SAMPLE_COUNT_1_BIT;
			desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			return desc;
		}

		VkAttachmentDescription CreateDepthAttachmentDescription() const
		{
			VkAttachmentDescription desc{};
			desc.format = GetDepthFormat();
			desc.samples = VK_SAMPLE_COUNT_1_BIT;
			desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			desc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			return desc;
		}

		VkAttachmentReference CreateColorAttachmentReference() const
		{
			VkAttachmentReference ref{};
			ref.attachment = 0;
			ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			return ref;
		}

		VkAttachmentReference CreateDepthAttachmentReference() const
		{
			VkAttachmentReference ref{};
			ref.attachment = 1;
			ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			return ref;
		}

		VkSubpassDescription CreateSubpassDescription() const
		{
			VkSubpassDescription desc{};
			desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			return desc;
		}

		container::vector<VkSubpassDependency> CreateSubpassDependencies() const
		{
			container::vector<VkSubpassDependency> dependencies{};

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask =
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask =
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			dependencies.emplace_back(dependency);

			return dependencies;
		}

		VkRenderPassCreateInfo CreateRenderPassInfo() const
		{
			VkRenderPassCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			return info;
		}

		VkRenderPass CreateRenderPass() const
		{
			VkRenderPass render_pass = nullptr;

			VkAttachmentReference color_attachment_reference = CreateColorAttachmentReference();
			VkAttachmentReference depth_attachment_reference = CreateDepthAttachmentReference();

			VkSubpassDescription subpass_description = CreateSubpassDescription();
			subpass_description.colorAttachmentCount = 1;
			subpass_description.pColorAttachments = &color_attachment_reference;
			subpass_description.pDepthStencilAttachment = &depth_attachment_reference;

			container::vector<VkSubpassDescription> subpass_descriptions = {subpass_description};

			container::vector<VkAttachmentDescription> attachment_descriptions = {CreateColorAttachmentDescription(),
																				  CreateDepthAttachmentDescription()};

			container::vector<VkSubpassDependency> subpass_dependencies = CreateSubpassDependencies();

			VkRenderPassCreateInfo render_pass_info = CreateRenderPassInfo();
			render_pass_info.attachmentCount = (ui32)attachment_descriptions.size();
			render_pass_info.pAttachments = attachment_descriptions.data();
			render_pass_info.subpassCount = (ui32)subpass_descriptions.size();
			render_pass_info.pSubpasses = subpass_descriptions.data();
			render_pass_info.dependencyCount = (ui32)subpass_dependencies.size();
			render_pass_info.pDependencies = subpass_dependencies.data();

			if (vkCreateRenderPass(GetDevice(), &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
			{
				render_pass = nullptr;
			}

			return render_pass;
		}

		VkFormat GetDepthFormat() const
		{
			return GetDevice().GetSupportedFormat(
				{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		}

		VulkanTexture* CreateDepthTexture()
		{
			VkImageCreateInfo depth_image_create_info = VulkanImage::CreateInfo();
			depth_image_create_info.extent.width = GetDevice().GetExtent().width;
			depth_image_create_info.extent.height = GetDevice().GetExtent().height;
			depth_image_create_info.format = GetDepthFormat();
			depth_image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			depth_image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			VkMemoryPropertyFlags depth_memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkImageViewCreateInfo depth_image_view_create_info = VulkanImageView::CreateInfo();
			depth_image_view_create_info.format = depth_image_create_info.format;
			depth_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			return memory::Create<VulkanTexture>(memory::DefaultTraitAllocator, GetDevice(), depth_image_create_info,
												 depth_memory_property_flags, depth_image_view_create_info);

			/*
			the following transition is covered in the render pass, but here it is for reference

			TransitionImageLayout(_depth_texture->GetImage(), depth_format, VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			*/
		}

		void Dispose()
		{
			if (_depth_texture)
			{
				memory::Destroy<VulkanTexture>(memory::DefaultTraitAllocator, _depth_texture);
				_depth_texture = nullptr;
			}

			if (_render_pass)
			{
				vkDestroyRenderPass(GetDevice(), _render_pass, nullptr);
				_render_pass = nullptr;
			}
		}

	public:
		static container::vector<VkClearValue> CreateClearValues()
		{
			container::vector<VkClearValue> values;

			VkClearValue clear_color;
			clear_color.color = {{0.0f, 0.0f, 0.0f, 1.0f}};

			VkClearValue depth_stencil;
			depth_stencil.depthStencil = {1.0f, 0};

			values.emplace_back(clear_color);
			values.emplace_back(depth_stencil);

			return values;
		}

		static VkRenderPassBeginInfo CreateRenderPassBeginInfo()
		{
			VkRenderPassBeginInfo info{};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderArea.offset = {0, 0};
			return info;
		}

		VulkanRenderPass(VulkanDevice& device):
			_device(device),
			_render_pass(CreateRenderPass()),
			_depth_texture(CreateDepthTexture()),
			_begin_render_pass(nullptr)
		{}

		~VulkanRenderPass()
		{
			vkDeviceWaitIdle(GetDevice());

			Dispose();
		}

		VulkanInstance& GetInstance()
		{
			return GetDevice().GetInstance();
		}

		const VulkanInstance& GetInstance() const
		{
			return GetDevice().GetInstance();
		}

		VulkanSurface& GetSurface()
		{
			return GetDevice().GetSurface();
		}

		const VulkanSurface& GetSurface() const
		{
			return GetDevice().GetSurface();
		}

		VulkanDevice& GetDevice()
		{
			return _device;
		}

		const VulkanDevice& GetDevice() const
		{
			return _device;
		}

		VulkanTexture& GetDepthTexture()
		{
			return *_depth_texture;
		}

		const VulkanTexture& GetDepthTexture() const
		{
			return *_depth_texture;
		}

		operator VkRenderPass() const
		{
			return _render_pass;
		}

		void Rebuild()
		{
			memory::Destroy<VulkanTexture>(memory::DefaultTraitAllocator, _depth_texture);
			_depth_texture = CreateDepthTexture();
		}

		void Begin(VkRenderPassBeginInfo& render_pass_begin_info, VulkanFrame& frame)
		{
			render_pass_begin_info.renderPass = _render_pass;
			render_pass_begin_info.renderArea.extent = GetDevice().GetExtent();

			if (_begin_render_pass)
				memory::Destroy<VulkanCommandBeginRenderPass>(memory::DefaultTraitAllocator, _begin_render_pass);

			_begin_render_pass = memory::Create<VulkanCommandBeginRenderPass>(
				memory::DefaultTraitAllocator, render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

			frame.StageCommand(*_begin_render_pass);
		}

		void End(VulkanFrame& frame)
		{
			frame.StageCommand(_end_render_pass);
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_RENDER_PASS_HPP */