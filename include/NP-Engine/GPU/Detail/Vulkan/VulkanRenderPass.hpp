//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_RENDER_PASS_HPP
#define NP_ENGINE_GPU_VULKAN_RENDER_PASS_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanAccess.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanImageResource.hpp"

namespace np::gpu::__detail
{
	struct VulkanSubpassImageResourceReference
	{
		ui32 framebufferImageViewIndex = UI32_MAX;
		VulkanImageResourceUsage usage = VulkanImageResourceUsage::None;

		VulkanSubpassImageResourceReference(const SubpassImageResourceReference& ref = {}):
			framebufferImageViewIndex(ref.framebufferImageViewIndex),
			usage(ref.usage)
		{}

		operator SubpassImageResourceReference() const
		{
			return {framebufferImageViewIndex, usage};
		}

		VkAttachmentReference GetVkAttachmentReference() const
		{
			return {framebufferImageViewIndex, usage.GetVkImageLayout()};
		}
	};

	struct VulkanSubpassDescription
	{
		con::vector<VulkanSubpassImageResourceReference> inputs{};
		con::vector<VulkanSubpassImageResourceReference> outputs{};
		con::vector<VulkanSubpassImageResourceReference> multisampleResolves{};
		con::vector<ui32> preserveFramebufferImageViewIndicies{};

		VulkanSubpassDescription(const SubpassDescription& other = {}):
			inputs(other.inputs.begin(), other.inputs.end()),
			outputs(other.outputs.begin(), other.outputs.end()),
			multisampleResolves(other.multisampleResolves.begin(), other.multisampleResolves.end())
		{
			preserveFramebufferImageViewIndicies.resize(other.preserveFramebufferImageViewIndicies.size());
			for (siz i = 0; i < preserveFramebufferImageViewIndicies.size(); i++)
				preserveFramebufferImageViewIndicies[i] = (ui32)other.preserveFramebufferImageViewIndicies[i];
		}

		operator SubpassDescription() const
		{
			return {{inputs.begin(), inputs.end()},
					{outputs.begin(), outputs.end()},
					{multisampleResolves.begin(), multisampleResolves.end()},
					{preserveFramebufferImageViewIndicies.begin(), preserveFramebufferImageViewIndicies.end()}};
		}

		/*
			NOTE: MUST BE CALLED ALONGSIDE THE Get____ResourceReferences METHODS
		*/
		VkSubpassDescription GetVkSubpassDescription() const
		{
			VkSubpassDescription description{};
			description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
			return description;
		}

		con::vector<VkAttachmentReference> GetVkInputVkAttachmentReferences() const
		{
			con::vector<VkAttachmentReference> refs(inputs.size());
			for (siz i = 0; i < refs.size(); i++)
				refs[i] = inputs[i].GetVkAttachmentReference();
			return refs;
		}

		con::vector<VkAttachmentReference> GetVkColorVkAttachmentReferences() const
		{
			con::vector<VkAttachmentReference> refs(outputs.size());
			for (siz i = 0; i < refs.size(); i++)
				refs[i] = outputs[i].GetVkAttachmentReference();
			return refs;
		}

		con::vector<VkAttachmentReference> GetVkResolveVkAttachmentReferences() const
		{
			con::vector<VkAttachmentReference> refs(multisampleResolves.size());
			for (siz i = 0; i < refs.size(); i++)
				refs[i] = multisampleResolves[i].GetVkAttachmentReference();
			return refs;
		}

		con::vector<ui32> GetPreserveFramebufferImageViewIndices() const
		{
			return preserveFramebufferImageViewIndicies;
		}
	};

	struct VulkanSubpassDependency
	{
		ui32 srcSubpassIndex = 0;
		ui32 dstSubpassIndex = 0;
		VulkanStage srcStage = VulkanStage::None;
		VulkanStage dstStage = VulkanStage::None;
		VulkanAccess srcAccess = VulkanAccess::None;
		VulkanAccess dstAccess = VulkanAccess::None;

		VulkanSubpassDependency(const SubpassDependency& other = {}):
			srcSubpassIndex(other.srcSubpassIndex == SIZ_MAX ? VK_SUBPASS_EXTERNAL : other.srcSubpassIndex),
			dstSubpassIndex(other.dstSubpassIndex == SIZ_MAX ? VK_SUBPASS_EXTERNAL : other.dstSubpassIndex),
			srcStage(other.srcStage),
			dstStage(other.dstStage),
			srcAccess(other.srcAccess),
			dstAccess(other.dstAccess)
		{}

		operator SubpassDependency() const
		{
			return {srcSubpassIndex, dstSubpassIndex, srcStage, dstStage, srcAccess, dstAccess};
		}

		VkSubpassDependency GetVkSubpassDependency() const
		{
			VkSubpassDependency dependency{};
			dependency.srcSubpass = srcSubpassIndex;
			dependency.dstSubpass = dstSubpassIndex;
			dependency.srcStageMask = srcStage.GetVkPipelineStageFlags();
			dependency.dstStageMask = dstStage.GetVkPipelineStageFlags();
			dependency.srcAccessMask = srcAccess.GetVkAccessFlags();
			dependency.dstAccessMask = dstAccess.GetVkAccessFlags();
			return dependency;
		}
	};

	class VulkanSubpassUsage : public SubpassUsage
	{
	public:
		VulkanSubpassUsage(ui32 value): SubpassUsage(value) {}

		VkSubpassContents GetVkSubpassContents() const
		{
			VkSubpassContents contents = VK_SUBPASS_CONTENTS_INLINE;

			if (Contains(HasSecondary))
				contents = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS;

			return contents;
		}
	};

	class VulkanRenderPass : public RenderPass
	{
	private:
		mem::sptr<VulkanDevice> _device;
		con::vector<VulkanImageResourceDescription> _image_descriptions;
		con::vector<VulkanSubpassDescription> _subpass_descriptions;
		con::vector<VulkanSubpassDependency> _subpass_dependencies;
		VkRenderPass _render_pass;

		static VkRenderPassCreateInfo CreateVkInfo()
		{
			VkRenderPassCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
			return info;
		}

		static VkRenderPass CreateVkRenderPass(mem::sptr<VulkanDevice> device,
											   const con::vector<VulkanImageResourceDescription>& descriptions,
											   const con::vector<VulkanSubpassDescription>& subpasses,
											   const con::vector<VulkanSubpassDependency>& dependencies)
		{
			con::vector<VkAttachmentDescription> attachment_descriptions(descriptions.size());
			for (siz i = 0; i < attachment_descriptions.size(); i++)
				attachment_descriptions[i] = descriptions[i].GetVkAttachmentDescription();

			con::vector<con::vector<VkAttachmentReference>> input_attachment_references(subpasses.size());
			con::vector<con::vector<VkAttachmentReference>> color_attachment_references(subpasses.size());
			con::vector<con::vector<VkAttachmentReference>> resolve_attachment_references(subpasses.size());
			con::vector<con::vector<ui32>> preserve_framebuffer_image_view_indicies(subpasses.size());
			con::vector<VkSubpassDescription> subpass_descriptions(subpasses.size());
			for (siz i = 0; i < subpass_descriptions.size(); i++)
			{
				input_attachment_references[i] = subpasses[i].GetVkInputVkAttachmentReferences();
				color_attachment_references[i] = subpasses[i].GetVkColorVkAttachmentReferences();
				resolve_attachment_references[i] = subpasses[i].GetVkResolveVkAttachmentReferences();
				preserve_framebuffer_image_view_indicies[i] = subpasses[i].GetPreserveFramebufferImageViewIndices();

				//TODO: should we check: if resolve_attachment_references[i] is not empty, then it's size must equal
				//color_attachment_references[i]

				subpass_descriptions[i] = subpasses[i].GetVkSubpassDescription();
				subpass_descriptions[i].inputAttachmentCount = input_attachment_references[i].size();
				subpass_descriptions[i].pInputAttachments =
					input_attachment_references[i].empty() ? nullptr : input_attachment_references[i].data();
				subpass_descriptions[i].colorAttachmentCount = color_attachment_references[i].size();
				subpass_descriptions[i].pColorAttachments =
					color_attachment_references[i].empty() ? nullptr : color_attachment_references[i].data();
				subpass_descriptions[i].pResolveAttachments =
					resolve_attachment_references[i].empty() ? nullptr : resolve_attachment_references[i].data();
				subpass_descriptions[i].preserveAttachmentCount = preserve_framebuffer_image_view_indicies[i].size();
				subpass_descriptions[i].pPreserveAttachments = preserve_framebuffer_image_view_indicies[i].empty()
					? nullptr
					: preserve_framebuffer_image_view_indicies[i].data();
			}

			con::vector<VkSubpassDependency> subpass_dependencies(dependencies.size());
			for (siz i = 0; i < subpass_dependencies.size(); i++)
				subpass_dependencies[i] = dependencies[i].GetVkSubpassDependency();

			VkRenderPassCreateInfo info = CreateVkInfo();
			info.attachmentCount = attachment_descriptions.size();
			info.pAttachments = attachment_descriptions.empty() ? nullptr : attachment_descriptions.data();
			info.subpassCount = subpass_descriptions.size();
			info.pSubpasses = subpass_descriptions.empty() ? nullptr : subpass_descriptions.data();
			info.dependencyCount = subpass_dependencies.size();
			info.pDependencies = subpass_dependencies.empty() ? nullptr : subpass_dependencies.data();

			VkRenderPass render_pass = nullptr;
			VkResult result = vkCreateRenderPass(*device->GetLogicalDevice(), &info, nullptr, &render_pass);
			return result == VK_SUCCESS ? render_pass : nullptr;
		}

	public:
		VulkanRenderPass(mem::sptr<Device> device, const con::vector<ImageResourceDescription>& descriptions,
						 const con::vector<SubpassDescription>& subpasses, const con::vector<SubpassDependency>& dependencies):
			_device(device),
			_image_descriptions(descriptions.begin(), descriptions.end()),
			_subpass_descriptions(subpasses.begin(), subpasses.end()),
			_subpass_dependencies(dependencies.begin(), dependencies.end()),
			_render_pass(CreateVkRenderPass(_device, _image_descriptions, _subpass_descriptions, _subpass_dependencies))
		{}

		~VulkanRenderPass()
		{
			if (_render_pass)
			{
				vkDestroyRenderPass(*_device->GetLogicalDevice(), _render_pass, nullptr);
				_render_pass = nullptr;
			}
		}

		operator VkRenderPass() const
		{
			return _render_pass;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _device;
		}

		virtual con::vector<ImageResourceDescription> GetImageResourceDescriptions() const override
		{
			return {_image_descriptions.begin(), _image_descriptions.end()};
		}

		virtual con::vector<SubpassDescription> GetSubpassDescriptions() const override
		{
			return {_subpass_descriptions.begin(), _subpass_descriptions.end()};
		}

		virtual con::vector<SubpassDependency> GetSubpassDepenedencies() const override
		{
			return {_subpass_dependencies.begin(), _subpass_dependencies.end()};
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_RENDER_PASS_HPP */