//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/9/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_FRAMEBUFFERS_HPP
#define NP_ENGINE_VULKAN_FRAMEBUFFERS_HPP

#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanRenderPass.hpp"
#include "VulkanSwapchain.hpp"

namespace np::gfx::rhi
{
	class VulkanFramebuffers
	{
	private:
		VulkanSwapchain& _swapchain;
		VulkanRenderPass& _render_pass;
		con::vector<VkFramebuffer> _framebuffers;

		VkFramebufferCreateInfo CreateFramebufferInfo()
		{
			VkFramebufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			info.width = GetSwapchain().GetExtent().width;
			info.height = GetSwapchain().GetExtent().height;
			info.layers = 1;
			return info;
		}

		con::vector<VkFramebuffer> CreateFramebuffers()
		{
			con::vector<VkFramebuffer> framebuffers(NP_ENGINE_VULKAN_MAX_FRAME_COUNT);

			for (siz i = 0; i < framebuffers.size(); i++)
			{
				con::vector<VkImageView> image_views{GetSwapchain().GetImageViews()[i],
													 GetRenderPass().GetDepthTexture().GetImageView()};

				VkFramebufferCreateInfo framebuffer_info = CreateFramebufferInfo();
				framebuffer_info.renderPass = GetRenderPass();
				framebuffer_info.attachmentCount = (ui32)image_views.size();
				framebuffer_info.pAttachments = image_views.data();

				if (vkCreateFramebuffer(GetDevice(), &framebuffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS)
				{
					framebuffers[i] = nullptr;
				}
			}

			return framebuffers;
		}

		void Dispose()
		{
			for (VkFramebuffer framebuffer : _framebuffers)
				vkDestroyFramebuffer(GetDevice(), framebuffer, nullptr);

			_framebuffers.clear();
		}

	public:
		VulkanFramebuffers(VulkanSwapchain& swapchain, VulkanRenderPass& render_pass):
			_swapchain(swapchain),
			_render_pass(render_pass),
			_framebuffers(CreateFramebuffers())
		{}

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

		VulkanInstance& GetInstance()
		{
			return GetRenderPass().GetInstance();
		}

		const VulkanInstance& GetInstance() const
		{
			return GetRenderPass().GetInstance();
		}

		VulkanSurface& GetSurface()
		{
			return GetRenderPass().GetSurface();
		}

		const VulkanSurface& GetSurface() const
		{
			return GetRenderPass().GetSurface();
		}

		VulkanDevice& GetDevice()
		{
			return GetRenderPass().GetDevice();
		}

		const VulkanDevice& GetDevice() const
		{
			return GetRenderPass().GetDevice();
		}

		VulkanRenderPass& GetRenderPass()
		{
			return _render_pass;
		}

		const VulkanRenderPass& GetRenderPass() const
		{
			return _render_pass;
		}

		VulkanSwapchain& GetSwapchain()
		{
			return _swapchain;
		}

		const VulkanSwapchain& GetSwapchain() const
		{
			return _swapchain;
		}

		void Rebuild()
		{
			Dispose();
			_framebuffers = CreateFramebuffers();
		}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_FRAMEBUFFERS_HPP */