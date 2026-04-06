//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/9/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_FRAMEBUFFER_HPP
#define NP_ENGINE_GPU_VULKAN_FRAMEBUFFER_HPP

#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Framebuffer.hpp"

#include "VulkanRenderPass.hpp"
#include "VulkanDevice.hpp"
#include "VulkanImageResourceView.hpp"

/*
	TODO: VkFramebufferCreateInfo issue - might be linux-specific - happens when resizing window
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
	class VulkanFramebuffer : public Framebuffer
	{
	private:
		mem::sptr<VulkanRenderPass> _render_pass;
		ui32 _width;
		ui32 _height;
		ui32 _layer_count;
		con::vector<mem::sptr<VulkanImageResourceView>> _views; //aka: attachments
		VkFramebuffer _framebuffer;

		static VkFramebufferCreateInfo CreateVkInfo(mem::sptr<VulkanRenderPass> render_pass, ui32 width, ui32 height,
													ui32 layer_count)
		{
			VkFramebufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			info.renderPass = *render_pass;
			info.width = width;
			info.height = height;
			info.layers = layer_count;
			return info;
		}

		static VkFramebuffer CreateVkFramebuffer(mem::sptr<VulkanRenderPass> render_pass, ui32 width, ui32 height,
												 ui32 layer_count, const con::vector<mem::sptr<VulkanImageResourceView>>& views)
		{
			con::vector<VkImageView> image_views(views.size());
			for (siz i = 0; i < image_views.size(); i++)
				image_views[i] = *views[i];

			VkFramebufferCreateInfo info = CreateVkInfo(render_pass, width, height, layer_count);
			info.attachmentCount = image_views.size();
			info.pAttachments = image_views.empty() ? nullptr : image_views.data();

			mem::sptr<VulkanDevice> device = render_pass->GetDevice();
			mem::sptr<VulkanInstance> instance = device->GetDetailInstance();
			VkFramebuffer framebuffer = nullptr;
			VkResult result = vkCreateFramebuffer(*device->GetLogicalDevice(), &info, instance->GetVulkanAllocationCallbacks(), &framebuffer);
			return result == VK_SUCCESS ? framebuffer : nullptr;
		}

	public:
		VulkanFramebuffer(mem::sptr<RenderPass> render_pass, siz width, siz height, siz layer_count,
						  const con::vector<mem::sptr<ImageResourceView>>& views):
			_render_pass(render_pass),
			_width(width),
			_height(height),
			_layer_count(layer_count),
			_views{views.begin(), views.end()},
			_framebuffer(CreateVkFramebuffer(_render_pass, _width, _height, _layer_count, _views))
		{}

		virtual ~VulkanFramebuffer()
		{
			if (_framebuffer)
			{
				mem::sptr<VulkanDevice> device = _render_pass->GetDevice();
				mem::sptr<VulkanInstance> instance = device->GetDetailInstance();
				vkDestroyFramebuffer(*device->GetLogicalDevice(), _framebuffer, instance->GetVulkanAllocationCallbacks());
				_framebuffer = nullptr;
			}
		}

		operator VkFramebuffer() const
		{
			return _framebuffer;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _render_pass->GetServices();
		}

		virtual mem::sptr<RenderPass> GetRenderPass() const override
		{
			return _render_pass;
		}

		virtual siz GetWidth() const override
		{
			return _width;
		}

		virtual siz GetHeight() const override
		{
			return _height;
		}

		virtual siz GetLayerCount() const override
		{
			return _layer_count;
		}

		virtual con::vector<mem::sptr<ImageResourceView>> GetImageResourceViews() const override
		{
			return {_views.begin(), _views.end()};
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_FRAMEBUFFER_HPP */