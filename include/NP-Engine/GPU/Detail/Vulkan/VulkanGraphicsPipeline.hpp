//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/26/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_RENDER_PIPELINE_HPP
#define NP_ENGINE_GPU_VULKAN_RENDER_PIPELINE_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanBlend.hpp"
#include "VulkanCompareOperation.hpp"
#include "VulkanDepthStencil.hpp"
#include "VulkanDynamic.hpp"
#include "VulkanLogic.hpp"
#include "VulkanMultisample.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanRasterization.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanScissor.hpp"
#include "VulkanShader.hpp"
#include "VulkanTopology.hpp"
#include "VulkanViewport.hpp"

namespace np::gpu::__detail
{
	class VulkanGraphicsPipeline : public GraphicsPipeline
	{
	private:
		mem::sptr<VulkanRenderPass> _render_pass;
		mem::sptr<VulkanPipelineResourceLayout> _layout;
		con::vector<mem::sptr<VulkanShader>> _shaders;
		con::vector<VulkanFormat> _input_vertex_formatting;
		con::vector<VulkanFormat> _input_instance_formatting;
		VulkanPrimitiveTopology _topology;
		ui32 _tessellation_patch_control_point_count;
		con::vector<VulkanViewport> _viewports;
		con::vector<VulkanScissor> _scissors;
		VulkanRasterization _rasterization;
		VulkanMultisample _multisample;
		VulkanDepthStencil _depth_stencil;
		VulkanBlend _blend;
		VulkanDynamicUsage _dynamic_usage;
		VkPipeline _pipeline;

		/*
			NOTE: set pVertexBindingDescriptions AND pVertexAttributeDescriptions alongside this
		*/
		static VkPipelineVertexInputStateCreateInfo GetVkVertexInputInfo()
		{
			VkPipelineVertexInputStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			return info;
		}

		static VkPipelineInputAssemblyStateCreateInfo GetVkInputAssemblyInfo(VulkanPrimitiveTopology topology)
		{
			VkPipelineInputAssemblyStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			info.topology = topology.GetVkPrimitiveTopology();
			info.primitiveRestartEnable = topology.Contains(VulkanPrimitiveTopology::Resettable) ? VK_TRUE : VK_FALSE;
			return info;
		}

		static VkPipelineTessellationStateCreateInfo GetVkTessellationInfo(ui32 patch_control_point_count)
		{
			VkPipelineTessellationStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			info.patchControlPoints = patch_control_point_count;
			return info;
		}

		/*
			NOTE: set pViewports and pScissors alongside this
		*/
		static VkPipelineViewportStateCreateInfo GetVkViewportInfo()
		{
			VkPipelineViewportStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			return info;
		}

		static VkPipelineRasterizationStateCreateInfo GetVkRasterizationInfo(const VulkanRasterization& rasterization)
		{
			VkPipelineRasterizationStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			info.lineWidth = 1.f; //TODO: do we want to support this? setting to 1.f is the best option (pg 259-260)

			info.depthClampEnable = rasterization.usage.Contains(VulkanRasterizationUsage::DepthClamp) ? VK_TRUE : VK_FALSE;
			info.rasterizerDiscardEnable = rasterization.usage.Contains(VulkanRasterizationUsage::Discard) ? VK_TRUE : VK_FALSE;
			info.polygonMode = rasterization.usage.GetVkPolygonMode();
			info.cullMode = rasterization.usage.GetVkCullModeFlags();
			info.frontFace = rasterization.usage.GetVkFrontFace();
			info.depthBiasEnable = rasterization.bias.IsVkEnabled();
			info.depthBiasConstantFactor = rasterization.bias.constantScalar;
			info.depthBiasClamp = rasterization.bias.clamp;
			info.depthBiasSlopeFactor = rasterization.bias.slopeScalar;
			return info;
		}

		/*
			NOTE: set pSampleMask alongside this
		*/
		static VkPipelineMultisampleStateCreateInfo GetVkMultisampleInfo(const VulkanMultisample& multisample)
		{
			VkPipelineMultisampleStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			info.rasterizationSamples = multisample.usage.GetVulkanSampleCount().GetVkSampleCountFlagBits();
			info.sampleShadingEnable = multisample.usage.IsVkSampleShading();
			info.minSampleShading = multisample.minSampleShadingPercentage;
			info.alphaToCoverageEnable = multisample.usage.IsVkAlphaToCoverage();
			info.alphaToOneEnable = multisample.usage.IsVkAlphaToOne();
			return info;
		}

		static VkPipelineDepthStencilStateCreateInfo GetVkDepthStencilInfo(const VulkanDepthStencil& depth_stencil)
		{
			VkPipelineDepthStencilStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			info.depthTestEnable = depth_stencil.usage.IsVkDepthTestEnabled();
			info.depthWriteEnable = depth_stencil.usage.IsVkDepthWriteEnabled();
			info.depthCompareOp = depth_stencil.depthCompareOperation.GetVkCompareOp();
			info.depthBoundsTestEnable = depth_stencil.usage.IsVkBoundsTestEnabled();
			info.stencilTestEnable = depth_stencil.usage.IsVkStencilTestEnabled();
			info.front = depth_stencil.frontState.GetVkStencilOpState();
			info.back = depth_stencil.backState.GetVkStencilOpState();
			info.minDepthBounds = depth_stencil.depthBounds.min;
			info.maxDepthBounds = depth_stencil.depthBounds.max;
			return info;
		}

		/*
			NOTE: set pAttachments alongside this
		*/
		static VkPipelineColorBlendStateCreateInfo GetVkColorBlendInfo(const VulkanBlend& blend)
		{
			VkPipelineColorBlendStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			info.logicOpEnable = !blend.logicOperation.ContainsAll(VulkanLogicOperation::None) ? VK_TRUE : VK_FALSE;
			info.logicOp = blend.logicOperation.GetVkLogicOp();
			info.blendConstants[0] = blend.constants.a;
			info.blendConstants[1] = blend.constants.b;
			info.blendConstants[2] = blend.constants.c;
			info.blendConstants[3] = blend.constants.d;
			return info;
		}

		/*
			NOTE: set pDynamicStates alongside this
		*/
		static VkPipelineDynamicStateCreateInfo GetVkDynamicInfo()
		{
			VkPipelineDynamicStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			return info;
		}

		static VkGraphicsPipelineCreateInfo CreateVkInfo(mem::sptr<VulkanRenderPass> render_pass, mem::sptr<VulkanPipelineResourceLayout> layout, VulkanPipelineUsage usage)
		{
			VkGraphicsPipelineCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			info.renderPass = *render_pass;
			info.layout = *layout;
			info.flags = usage.GetVkPipelineCreateFlags();
			return info;
		}

		static VkPipeline CreateVkPipeline(mem::sptr<VulkanRenderPass> render_pass,
			VulkanPipelineUsage usage,
			mem::sptr<VulkanPipelineResourceLayout> layout,
			const con::vector<mem::sptr<VulkanShader>>& shaders,
			const con::vector<VulkanFormat>& input_vertex_formatting,
			const con::vector<VulkanFormat>& input_instance_formatting,
			VulkanPrimitiveTopology topology, 
			ui32 tessellation_patch_control_point_count,
			const con::vector<VulkanViewport>& viewports,
			const con::vector<VulkanScissor>& scissors,
			const VulkanRasterization& rasterization,
			const VulkanMultisample& multisample,
			const VulkanDepthStencil& depth_stencil,
			const VulkanBlend& blend,
			VulkanDynamicUsage dynamic_usage,
			mem::sptr<VulkanPipelineCache> cache)
		{
			bl vertex_shader_found = false;
			con::vector<VkPipelineShaderStageCreateInfo> shader_infos(shaders.size());
			for (siz i = 0; i < shader_infos.size(); i++)
			{
				shader_infos[i] = shaders[i]->GetVkPipelineShaderStageCreateInfo();
				vertex_shader_found |= shaders[i]->GetStage().ContainsAny(VulkanStage::Vertex); //TODO: should this be ContainsAll?
			}

			NP_ENGINE_ASSERT(vertex_shader_found, "VulkanGraphicsPipeline requires a vertex shader");

			//vertex info
			ui32 vertex_stride = 0;
			for (const VulkanFormat& format : input_vertex_formatting)
				vertex_stride += format.GetComponentSize() * format.GetComponentCount();

			ui32 instance_stride = 0;
			for (const VulkanFormat& format : input_instance_formatting)
				instance_stride += format.GetComponentSize() * format.GetComponentCount();

			con::vector<VkVertexInputBindingDescription> vertex_binding_descs{};
			const ui32 vertex_binding = vertex_binding_descs.size();
			if (!input_vertex_formatting.empty())
				vertex_binding_descs.emplace_back(VkVertexInputBindingDescription{ vertex_binding, vertex_stride, VK_VERTEX_INPUT_RATE_VERTEX });
			
			const ui32 instance_binding = vertex_binding_descs.size();
			if (!input_instance_formatting.empty())
				vertex_binding_descs.emplace_back(VkVertexInputBindingDescription{ instance_binding, instance_stride, VK_VERTEX_INPUT_RATE_INSTANCE });
			
			ui32 attribute_location = 0;
			ui32 attribute_offset = 0;
			con::vector<VkVertexInputAttributeDescription> vertex_attribute_descs{};
			for (const VulkanFormat& format : input_vertex_formatting)
			{
				vertex_attribute_descs.emplace_back(VkVertexInputAttributeDescription{ attribute_location++, vertex_binding, format.GetVkFormat(), attribute_offset });
				attribute_offset += format.GetComponentSize() * format.GetComponentCount();
			}

			attribute_offset = 0;
			for (const VulkanFormat& format : input_instance_formatting)
			{
				vertex_attribute_descs.emplace_back(VkVertexInputAttributeDescription{ attribute_location++, instance_binding, format.GetVkFormat(), attribute_offset });
				attribute_offset += format.GetComponentSize() * format.GetComponentCount();
			}

			VkPipelineVertexInputStateCreateInfo vertex_info = GetVkVertexInputInfo();
			vertex_info.vertexBindingDescriptionCount = vertex_binding_descs.size();
			vertex_info.pVertexBindingDescriptions = vertex_binding_descs.empty() ? nullptr : vertex_binding_descs.data();
			vertex_info.vertexAttributeDescriptionCount = vertex_attribute_descs.size();
			vertex_info.pVertexAttributeDescriptions = vertex_attribute_descs.empty() ? nullptr : vertex_attribute_descs.data();

			//input info
			VkPipelineInputAssemblyStateCreateInfo input_info = GetVkInputAssemblyInfo(topology);
			/*
				TODO: ^ some topology requires certain shaders, etc:
					- [ ] topology with adjanency requires a geometry shader
					- [ ] topology patch list requires tessellation
			*/

			//tessellation info
			const bl tessellation_enabled = usage.Contains(VulkanPipelineUsage::Tessellation) && input_info.topology == VK_PRIMITIVE_TOPOLOGY_PATCH_LIST; //TODO: consider if we have a tessllation shaders too
			VkPipelineTessellationStateCreateInfo tessellation_info = GetVkTessellationInfo(tessellation_patch_control_point_count); //TODO: support patch control points for tessellation

			//viewport infos
			con::vector<VkViewport> vk_viewports(viewports.size());
			for (siz i = 0; i < vk_viewports.size(); i++)
				vk_viewports[i] = viewports[i].GetVkViewport();

			con::vector<VkRect2D> vk_scissors(scissors.size());
			for (siz i = 0; i < vk_scissors.size(); i++)
				vk_scissors[i] = scissors[i].GetVkRect2D();

			//TODO: vk_viewports.size() MUST equal vk_scissors.size() -- the index vulkan uses per viewport is used to pull the cooresponding scissor
			//TODO: ^ I'm suspicious if vk_scissors.empty(), then scissor testing is disabled? thus pScissors below could be nullptr? maybe not...
			//TODO: ^ it is common to use a viewport that is the same size of the framebuffer we use

			VkPipelineViewportStateCreateInfo viewport_info = GetVkViewportInfo();
			viewport_info.viewportCount = vk_viewports.size();
			viewport_info.pViewports = vk_viewports.empty() ? nullptr : vk_viewports.data();
			viewport_info.scissorCount = vk_scissors.size();
			viewport_info.pScissors = vk_scissors.empty() ? nullptr : vk_scissors.data();

			//rasterization info
			VkPipelineRasterizationStateCreateInfo rasterization_info = GetVkRasterizationInfo(rasterization);

			//TODO: we may not need all the above? I wonder how many we can just make nullptr - to clarify, then below can be nullptr

			//multisample
			const bl multisample_enable = !multisample.usage.ContainsAll(VulkanMultisampleUsage::None);
			VkPipelineMultisampleStateCreateInfo multisample_info = GetVkMultisampleInfo(multisample);
			//TODO: multisample_info.pSampleMask = nullptr;
			//multisample_info.pSampleMask = multisample.sampleMasks.empty() ? nullptr : multisample.sampleMasks.data();
			
			//depth stencil
			const bl depth_stencil_enabled = !depth_stencil.usage.ContainsAll(VulkanDepthStencilUsage::None);
			VkPipelineDepthStencilStateCreateInfo depth_stencil_info = GetVkDepthStencilInfo(depth_stencil);
			
			//color blending
			con::vector<VkPipelineColorBlendAttachmentState> blend_states(blend.resourceBlendings.size());
			for (siz i = 0; i < blend_states.size(); i++)
				blend_states[i] = blend.resourceBlendings[i].GetVkPipelineColorBlendAttachmentState();

			VkPipelineColorBlendStateCreateInfo color_blend_info = GetVkColorBlendInfo(blend);
			color_blend_info.attachmentCount = blend_states.size();
			color_blend_info.pAttachments = blend_states.empty() ? nullptr : blend_states.data();
			const bl color_blend_enable = usage.Contains(VulkanPipelineUsage::ColorBlend);
			
			//dynamic info
			const con::vector<VkDynamicState> dynamic_states = dynamic_usage.GetVkDynamicStates();
			VkPipelineDynamicStateCreateInfo dynamic_info = GetVkDynamicInfo();
			dynamic_info.dynamicStateCount = dynamic_states.size();
			dynamic_info.pDynamicStates = dynamic_states.empty() ? nullptr : dynamic_states.data();
			const bl dynamic_enabled = usage.Contains(VulkanPipelineUsage::Dynamic) && !dynamic_states.empty();

			//TODO: consider pipeline usage more above to enable/disable things in the info below

			VkGraphicsPipelineCreateInfo info = CreateVkInfo(render_pass, layout, usage);
			info.stageCount = shader_infos.size();
			info.pStages = shader_infos.empty() ? nullptr : shader_infos.data();
			info.pVertexInputState = &vertex_info;
			info.pInputAssemblyState = &input_info;
			info.pTessellationState = tessellation_enabled ? &tessellation_info : nullptr;
			info.pViewportState = &viewport_info;
			info.pRasterizationState = &rasterization_info;
			info.pMultisampleState = multisample_enable ? &multisample_info : nullptr;
			info.pDepthStencilState = depth_stencil_enabled ? &depth_stencil_info : nullptr;
			info.pColorBlendState = color_blend_enable ? &color_blend_info : nullptr;
			info.pDynamicState = dynamic_enabled ? &dynamic_info : nullptr;

			info.subpass = 0; //TODO: support caller to set this?

			mem::sptr<VulkanDevice> device = render_pass->GetDevice();
			VkPipeline pipeline = nullptr;
			VkResult result = vkCreateGraphicsPipelines(*device->GetLogicalDevice(), nullptr, 1, &info, nullptr, &pipeline);
			return result == VK_SUCCESS ? pipeline : nullptr;
		}

		static con::vector<mem::sptr<VulkanShader>> ChooseShaders(const con::vector<mem::sptr<Shader>>& shaders)
		{
			con::vector<mem::sptr<VulkanShader>> chosen{}; //TODO: we need to filter to the only 5 accepted shader stages --are we sure?

			for (mem::sptr<Shader> shader : shaders)
			{
				bl found = false;
				for (siz i = 0; !found && i < chosen.size(); i++)
					found |= chosen[i]->GetStage() == shader->GetStage(); //TODO: this check is not entirely accurate

				if (!found)
				{
					mem::sptr<VulkanShader> choice = DetailObject::EnsureIsDetailType(shader, DetailType::Vulkan);
					if (choice)
						chosen.emplace_back(choice);
				}
			}

			//TODO: do we want to sort the chosen shaders?

			return chosen;
		}

	public:
		VulkanGraphicsPipeline(mem::sptr<RenderPass> render_pass,
			PipelineUsage usage,
			mem::sptr<PipelineResourceLayout> layout,
			const con::vector<mem::sptr<Shader>>& shaders,
			const con::vector<Format>& input_vertex_formatting,
			const con::vector<Format>& input_instance_formatting,
			PrimitiveTopology topology,
			siz tessellation_patch_control_point_count,
			const con::vector<Viewport>& viewports,
			const con::vector<Scissor>& scissors,
			const Rasterization& rasterization,
			const Multisample& multisample,
			const DepthStencil& depth_stencil,
			const Blend& blend,
			DynamicUsage dynamic_usage,
			mem::sptr<PipelineCache> cache):
			_render_pass(DetailObject::EnsureIsDetailType(render_pass, DetailType::Vulkan)),
			_layout(DetailObject::EnsureIsDetailType(layout, DetailType::Vulkan)),
			_shaders(ChooseShaders(shaders)),
			_input_vertex_formatting(input_vertex_formatting.begin(), input_vertex_formatting.end()),
			_input_instance_formatting(input_instance_formatting.begin(), input_instance_formatting.end()),
			_topology(topology),
			_tessellation_patch_control_point_count(tessellation_patch_control_point_count),
			_viewports(viewports.begin(), viewports.end()),
			_scissors(scissors.begin(), scissors.end()),
			_rasterization(rasterization),
			_multisample(multisample),
			_depth_stencil(depth_stencil),
			_blend(blend),
			_dynamic_usage(dynamic_usage),
			_pipeline(CreateVkPipeline(_render_pass,
				VulkanPipelineUsage{ usage },
				_layout,
				_shaders,
				_input_vertex_formatting,
				_input_instance_formatting,
				_topology,
				_tessellation_patch_control_point_count,
				_viewports,
				_scissors,
				_rasterization,
				_multisample,
				_depth_stencil,
				_blend,
				_dynamic_usage,
				cache))
		{}

		virtual ~VulkanGraphicsPipeline()
		{
			if (_pipeline)
			{
				mem::sptr<VulkanDevice> device = _render_pass->GetDevice();
				vkDestroyPipeline(*device->GetLogicalDevice(), _pipeline, nullptr);
				_pipeline = nullptr;
			}
		}

		operator VkPipeline() const
		{
			return _pipeline;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _render_pass->GetServices();
		}

		virtual mem::sptr<PipelineResourceLayout> GetPipelineResourceLayout() const override
		{
			return _layout;
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _render_pass->GetDevice();
		}



		//void Rebuild()
		//{
		//	mem::sptr<RenderContext> render_context = GetRenderContext();
		//	VulkanRenderContext& vulkan_render_context = (VulkanRenderContext&)(*render_context);

		//	_descriptor_sets->Rebuild(vulkan_render_context.GetFramesCount());

		//	// TODO: pretty sure we only need to rebuild ubo stuff?? Nah I think everything needs to be rebuilt....??

		//	Dispose();

		//	_meta_values.resize(vulkan_render_context.GetFramesCount());
		//	_meta_value_buffers = CreateMetaValueBuffers(GetServices(), render_context);
		//	_meta_value_descriptor_infos = CreateMetaValueDescriptorInfos(render_context, _meta_value_buffers);
		//	_meta_value_descriptor_writers = CreateMetaValueDescriptorWriters(render_context, _meta_value_descriptor_infos);
		//	_pipeline_layout = CreatePipelineLayout(render_context, _descriptor_set_layout);
		//	_pipeline = CreatePipeline(_properties, _pipeline_layout);
		//}



	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_RENDER_PIPELINE_HPP */