//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/9/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_FRAMEBUFFERS_HPP
#define NP_ENGINE_VULKAN_FRAMEBUFFERS_HPP

#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanRenderPass.hpp"

/*
	TODO: VkFramebufferCreateInfo issue - might be linux-specific
		[critical, NP_ENGINE_LOG, 2023-10-23 21:44:37.651, pid:10233, tid:10233]
		NP Validation Layer: [[VUID-VkFramebufferCreateInfo-flags-04533, -26532824]]
		Validation Error: [ VUID-VkFramebufferCreateInfo-flags-04533 ] Object 0: handle = 0x7fffb53c3140,
		type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0xfe6b2428 | vkCreateFramebuffer(): VkFramebufferCreateInfo
		attachment #0 mip level 0 has width (838) smaller than the corresponding framebuffer width (839). The
		Vulkan spec states: If flags does not include VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT, each element of
		pAttachments that is used as an input, color, resolve, or depth/stencil attachment by renderPass must
		have been created with a VkImageCreateInfo::extent.width greater than or equal to width
		(https://vulkan.lunarg.com/doc/view/1.3.250.1/linux/1.3-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-flags-04533)

	Prexisting:
		[critical, NP_ENGINE_LOG, 2023-10-23 21:48:10.549, pid:11242, tid:11242]
		NP Validation Layer: [[VUID-VkFramebufferCreateInfo-flags-04534, 2011804586]]
		Validation Error: [ VUID-VkFramebufferCreateInfo-flags-04534 ] Object 0: handle = 0x7fffb13c35e0,
		type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x77e9b3aa | vkCreateFramebuffer(): VkFramebufferCreateInfo
		attachment #0 mip level 0 has height (698) smaller than the corresponding framebuffer height (699). The
		Vulkan spec states: If flags does not include VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT, each element of
		pAttachments that is used as an input, color, resolve, or depth/stencil attachment by renderPass must
		have been created with a VkImageCreateInfo::extent.height greater than or equal to height
		(https://vulkan.lunarg.com/doc/view/1.3.250.1/linux/1.3-extensions/vkspec.html#VUID-VkFramebufferCreateInfo-flags-04534)
*/

namespace np::gpu::__detail
{
	class VulkanFramebuffers : public Framebuffers
	{
	private:
		con::vector<VkFramebuffer> _framebuffers;

		static VkFramebufferCreateInfo CreateFramebufferInfo(mem::sptr<RenderContext> context)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);
			VkFramebufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			info.width = render_context.GetExtent().width;
			info.height = render_context.GetExtent().height;
			info.layers = 1;
			return info;
		}

		static con::vector<VkFramebuffer> CreateFramebuffers(mem::sptr<RenderPass> pass)
		{
			VulkanRenderPass& render_pass = (VulkanRenderPass&)(*pass);
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*pass->GetRenderContext());
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*pass->GetRenderContext()->GetRenderDevice());

			con::vector<VkFramebuffer> framebuffers(render_context.GetFramesCount());

			for (siz i = 0; i < framebuffers.size(); i++)
			{
				con::vector<VkImageView> image_views{render_context.GetFrames()[i].imageView,
													 render_pass.GetDepthTexture()->GetImageView()};

				VkFramebufferCreateInfo framebuffer_info = CreateFramebufferInfo(pass->GetRenderContext());
				framebuffer_info.renderPass = render_pass;
				framebuffer_info.attachmentCount = (ui32)image_views.size();
				framebuffer_info.pAttachments = image_views.data();

				if (vkCreateFramebuffer(render_device, &framebuffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS)
					framebuffers[i] = nullptr;
			}

			return framebuffers;
		}

		void Dispose()
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*GetRenderPass()->GetRenderContext()->GetRenderDevice());

			for (VkFramebuffer framebuffer : _framebuffers)
				vkDestroyFramebuffer(render_device, framebuffer, nullptr);

			_framebuffers.clear();
		}

	public:
		VulkanFramebuffers(mem::sptr<RenderPass> pass): Framebuffers(pass), _framebuffers(CreateFramebuffers(pass)) {}

		~VulkanFramebuffers()
		{
			Dispose();
		}

		operator const con::vector<VkFramebuffer>&() const
		{
			return _framebuffers;
		}

		VkFramebuffer operator[](i32 index) const
		{
			return _framebuffers[index];
		}

		void Rebuild()
		{
			Dispose();
			_framebuffers = CreateFramebuffers(GetRenderPass());
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_FRAMEBUFFERS_HPP */