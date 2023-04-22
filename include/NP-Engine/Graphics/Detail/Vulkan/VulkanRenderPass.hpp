//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_PASS_HPP
#define NP_ENGINE_VULKAN_RENDER_PASS_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/Interface/Interface.hpp"

#include "VulkanCommands.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanTexture.hpp"

namespace np::gfx::__detail
{
	class VulkanRenderPass : public RenderPass
	{
	private:
		VkRenderPass _render_pass;
		mem::sptr<VulkanTexture> _depth_texture;
		mem::sptr<VulkanCommandBeginRenderPass> _begin_render_pass;
		mem::sptr<VulkanCommandEndRenderPass> _end_render_pass;

		static VkAttachmentDescription CreateColorAttachmentDescription(mem::sptr<RenderDevice> device)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			VkAttachmentDescription desc{};
			desc.format = render_device.GetSurfaceFormat().format;
			desc.samples = VK_SAMPLE_COUNT_1_BIT;
			desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			return desc;
		}

		static VkAttachmentDescription CreateDepthAttachmentDescription(VkFormat depth_format)
		{
			VkAttachmentDescription desc{};
			desc.format = depth_format;
			desc.samples = VK_SAMPLE_COUNT_1_BIT;
			desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			desc.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			return desc;
		}

		static VkAttachmentReference CreateColorAttachmentReference()
		{
			VkAttachmentReference ref{};
			ref.attachment = 0;
			ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			return ref;
		}

		static VkAttachmentReference CreateDepthAttachmentReference()
		{
			VkAttachmentReference ref{};
			ref.attachment = 1;
			ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			return ref;
		}

		static VkSubpassDescription CreateSubpassDescription()
		{
			VkSubpassDescription desc{};
			desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			return desc;
		}

		static con::vector<VkSubpassDependency> CreateSubpassDependencies()
		{
			con::vector<VkSubpassDependency> dependencies{};

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

		static VkRenderPassCreateInfo CreateRenderPassInfo()
		{
			VkRenderPassCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			return info;
		}

		static VkRenderPass CreateRenderPass(mem::sptr<RenderDevice> device)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			VkRenderPass render_pass = nullptr;

			VkAttachmentReference color_attachment_reference = CreateColorAttachmentReference();
			VkAttachmentReference depth_attachment_reference = CreateDepthAttachmentReference();

			VkSubpassDescription subpass_description = CreateSubpassDescription();
			subpass_description.colorAttachmentCount = 1;
			subpass_description.pColorAttachments = &color_attachment_reference;
			subpass_description.pDepthStencilAttachment = &depth_attachment_reference;

			con::vector<VkSubpassDescription> subpass_descriptions = {subpass_description};

			con::vector<VkAttachmentDescription> attachment_descriptions = { CreateColorAttachmentDescription(device),
																			CreateDepthAttachmentDescription(GetDepthFormat(device)) };

			con::vector<VkSubpassDependency> subpass_dependencies = CreateSubpassDependencies();

			VkRenderPassCreateInfo render_pass_info = CreateRenderPassInfo();
			render_pass_info.attachmentCount = (ui32)attachment_descriptions.size();
			render_pass_info.pAttachments = attachment_descriptions.data();
			render_pass_info.subpassCount = (ui32)subpass_descriptions.size();
			render_pass_info.pSubpasses = subpass_descriptions.data();
			render_pass_info.dependencyCount = (ui32)subpass_dependencies.size();
			render_pass_info.pDependencies = subpass_dependencies.data();

			if (vkCreateRenderPass(render_device, &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
				render_pass = nullptr;

			return render_pass;
		}

		static VkFormat GetDepthFormat(mem::sptr<RenderDevice> device)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*device);
			return render_device.GetLogicalDevice()->GetSupportedFormat(
				{VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL,
				VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
		}

		static mem::sptr<VulkanTexture> CreateDepthTexture(mem::sptr<RenderContext> context)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*context->GetRenderDevice());

			VkImageCreateInfo depth_image_create_info = VulkanImage::CreateInfo();
			depth_image_create_info.extent.width = render_context.GetExtent().width;
			depth_image_create_info.extent.height = render_context.GetExtent().height;
			depth_image_create_info.format = GetDepthFormat(context->GetRenderDevice());
			depth_image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
			depth_image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
			VkMemoryPropertyFlags depth_memory_property_flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			VkImageViewCreateInfo depth_image_view_create_info = VulkanImageView::CreateInfo();
			depth_image_view_create_info.format = depth_image_create_info.format;
			depth_image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			return mem::create_sptr<VulkanTexture>(context->GetServices()->GetAllocator(), 
				render_device.GetCommandPool(), depth_image_create_info, depth_memory_property_flags,
				depth_image_view_create_info);
			/*
			the following transition is covered in the render pass, but here it is for reference

			TransitionImageLayout(_depth_texture->GetImage(), depth_format, VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
			*/
		}

		void Dispose()
		{
			if (_depth_texture)
				_depth_texture.reset();

			if (_render_pass)
			{
				VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*GetRenderContext()->GetRenderDevice());
				vkDestroyRenderPass(render_device, _render_pass, nullptr);
				_render_pass = nullptr;
			}
		}

	public:
		static con::vector<VkClearValue> CreateClearValues()
		{
			con::vector<VkClearValue> values;

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

		static mem::sptr<VulkanCommandEndRenderPass> CreateEndRenderPassCommand(mem::sptr<srvc::Services> services)
		{
			return mem::create_sptr<VulkanCommandEndRenderPass>(services->GetAllocator());
		}

		VulkanRenderPass(mem::sptr<RenderContext> context):
			RenderPass(context),
			_render_pass(CreateRenderPass(context->GetRenderDevice())),
			_depth_texture(CreateDepthTexture(context)),
			_begin_render_pass(nullptr),
			_end_render_pass(CreateEndRenderPassCommand(GetServices()))
		{}

		~VulkanRenderPass()
		{
			Dispose();
		}

		mem::sptr<VulkanTexture> GetDepthTexture() const
		{
			return _depth_texture;
		}

		operator VkRenderPass() const
		{
			return _render_pass;
		}

		void Rebuild()
		{
			_depth_texture = CreateDepthTexture(GetRenderContext());
		}

		void Begin(VkRenderPassBeginInfo& render_pass_begin_info, mem::sptr<CommandStaging> staging)
		{
			NP_ENGINE_ASSERT(staging, "Vulkan requires CommandStaging to be valid");

			VulkanRenderContext& render_context = (VulkanRenderContext&)(*GetRenderContext());
			render_pass_begin_info.renderPass = _render_pass;
			render_pass_begin_info.renderArea.extent = render_context.GetExtent();
			_begin_render_pass = mem::create_sptr<VulkanCommandBeginRenderPass>(GetServices()->GetAllocator(), render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
			staging->Stage(_begin_render_pass);
		}

		void End(mem::sptr<CommandStaging> staging)
		{
			NP_ENGINE_ASSERT(staging, "Vulkan requires CommandStaging to be valid");
			staging->Stage(_end_render_pass);
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_RENDER_PASS_HPP */