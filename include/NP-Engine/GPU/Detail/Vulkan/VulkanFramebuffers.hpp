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

			con::vector<VkFramebuffer> framebuffers(render_context.GetFramesInFlightCount());

			for (siz i = 0; i < framebuffers.size(); i++)
			{
				con::vector<VkImageView> image_views{render_context.GetImageViews()[i],
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