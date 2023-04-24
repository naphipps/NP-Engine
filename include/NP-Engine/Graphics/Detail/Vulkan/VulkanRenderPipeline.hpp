//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/26/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDER_PIPELINE_HPP
#define NP_ENGINE_VULKAN_RENDER_PIPELINE_HPP

#include "NP-Engine/Graphics/Interface/Interface.hpp"

#include "VulkanDescriptorSetLayout.hpp"
#include "VulkanDescriptorSets.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanCommands.hpp"
#include "VulkanRenderShader.hpp"
#include "VulkanTexture.hpp"
#include "VulkanVertex.hpp"
#include "VulkanFramebuffers.hpp"

namespace np::gfx::__detail
{
	class VulkanRenderPipeline : public RenderPipeline
	{
	private:
		mem::sptr<VulkanDescriptorSetLayout> _descriptor_set_layout;
		mem::sptr<VulkanDescriptorSets> _descriptor_sets;
		con::vector<mem::sptr<VulkanBuffer>> _meta_value_buffers;
		con::vector<VkDescriptorBufferInfo> _meta_value_descriptor_infos;
		con::vector<VkWriteDescriptorSet> _meta_value_descriptor_writers;

		mem::sptr<VulkanSampler> _sampler; // TODO: our RenderPipeline::Properties could store this
		VkPipelineLayout _pipeline_layout;
		VkPipeline _pipeline;
		mem::sptr<VulkanCommandBindPipeline> _bind_pipeline;
		siz _bind_descriptor_sets_command_slot;
		con::vector<VkDescriptorSet> _bound_descriptor_sets;
		mem::sptr<VulkanCommandBindDescriptorSets> _bind_descriptor_sets;

		static mem::sptr<VulkanDescriptorSetLayout> CreateDescriptorSetLayout(mem::sptr<srvc::Services> services, mem::sptr<VulkanLogicalDevice> device)
		{
			return mem::create_sptr<VulkanDescriptorSetLayout>(services->GetAllocator(), device);
		}

		static mem::sptr<VulkanDescriptorSets> CreateDescriptorSets(mem::sptr<srvc::Services> services, 
			mem::sptr<RenderContext> context, 
			mem::sptr<VulkanDescriptorSetLayout> descriptor_set_layout)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*render_context.GetRenderDevice());

			return mem::create_sptr<VulkanDescriptorSets>(services->GetAllocator(), render_device.GetLogicalDevice(), render_context.GetFramesInFlightCount(), descriptor_set_layout);
		}

		static mem::sptr<VulkanBuffer> CreateBuffer(mem::sptr<srvc::Services> services,
			mem::sptr<VulkanCommandPool> command_pool, VkDeviceSize size, 
			VkBufferUsageFlags buffer_usage_flags,
			VkMemoryPropertyFlags memory_property_flags)
		{
			VkBufferCreateInfo info = VulkanBuffer::CreateInfo();
			info.size = size;
			info.usage = buffer_usage_flags;

			return mem::create_sptr<VulkanBuffer>(services->GetAllocator(), command_pool, info, memory_property_flags);
		}

		static con::vector<mem::sptr<VulkanBuffer>> CreateMetaValueBuffers(mem::sptr<srvc::Services> services, mem::sptr<RenderContext> context)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*render_context.GetRenderDevice());
			
			con::vector<mem::sptr<VulkanBuffer>> buffers(render_context.GetFramesInFlightCount());

			for (siz i = 0; i < buffers.size(); i++)
			{
				buffers[i] = CreateBuffer(services, render_device.GetCommandPool(), sizeof(PipelineMetaValues), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			return buffers;
		}

		static con::vector<VkDescriptorBufferInfo> CreateMetaValueDescriptorInfos(mem::sptr<RenderContext> context, const con::vector<mem::sptr<VulkanBuffer>>& _meta_value_buffers)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);
			con::vector<VkDescriptorBufferInfo> infos(render_context.GetFramesInFlightCount());

			for (siz i = 0; i < infos.size(); i++)
			{
				infos[i].buffer = *_meta_value_buffers[i];
				infos[i].offset = 0;
				infos[i].range = sizeof(PipelineMetaValues);
			}

			return infos;
		}

		static con::vector<VkWriteDescriptorSet> CreateMetaValueDescriptorWriters(mem::sptr<RenderContext> context, const con::vector<VkDescriptorBufferInfo>& _meta_value_descriptor_infos)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);
			con::vector<VkWriteDescriptorSet> writers(render_context.GetFramesInFlightCount(),
				{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET });

			for (siz i = 0; i < writers.size(); i++)
			{
				writers[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				writers[i].pBufferInfo = mem::AddressOf(_meta_value_descriptor_infos[i]);
				writers[i].descriptorCount = 1;
			}

			return writers;
		}

		static mem::sptr<VulkanSampler> CreateSampler(mem::sptr<srvc::Services> services, mem::sptr<RenderContext> render_context, VkSamplerCreateInfo info)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*render_context->GetRenderDevice());
			return mem::create_sptr<VulkanSampler>(services->GetAllocator(), render_device.GetLogicalDevice(), info);
		}

		static VkPushConstantRange CreatePushConstantRange()
		{
			VkPushConstantRange range{};
			range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			range.offset = 0;
			range.size = sizeof(RenderableMetaValues);
			return range;
		}

		static VkPipelineLayoutCreateInfo CreatePipelineLayoutInfo()
		{
			VkPipelineLayoutCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			info.setLayoutCount = 0; // Optional
			info.pSetLayouts = nullptr; // Optional
			return info;
		}

		static VkPipelineLayout CreatePipelineLayout(mem::sptr<RenderContext> render_context, mem::sptr<VulkanDescriptorSetLayout> descriptor_set_layout)
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*render_context->GetRenderDevice());

			VkPipelineLayout layout = nullptr;
			VkDescriptorSetLayout descriptor_layout = *descriptor_set_layout;
			VkPushConstantRange push_constant_range = CreatePushConstantRange();
			VkPipelineLayoutCreateInfo create_info = CreatePipelineLayoutInfo();
			// TODO: setLayoutCount must be less than or equal to VkPhysicalDeviceLimits::maxBoundDescriptorSets
			create_info.setLayoutCount = 1;
			create_info.pSetLayouts = &descriptor_layout;
			create_info.pushConstantRangeCount = 1;
			create_info.pPushConstantRanges = &push_constant_range;

			if (vkCreatePipelineLayout(*render_device.GetLogicalDevice(), &create_info, nullptr, &layout) != VK_SUCCESS)
				layout = nullptr;

			return layout;
		}

		static VkPipelineVertexInputStateCreateInfo CreatePipelineVertexInputStateInfo()
		{
			VkPipelineVertexInputStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			return info;
		}

		static VkPipelineShaderStageCreateInfo CreatePipelineShaderStageInfo()
		{
			VkPipelineShaderStageCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			return info;
		}

		static con::vector<VkPipelineShaderStageCreateInfo> CreateShaderStages(RenderPipeline::Properties properties)
		{
			VulkanRenderShader& vertex_shader = (VulkanRenderShader&)(*properties.vertexShader);
			VulkanRenderShader& fragment_shader = (VulkanRenderShader&)(*properties.fragmentShader);

			con::vector<VkPipelineShaderStageCreateInfo> stages;

			VkPipelineShaderStageCreateInfo vertex_stage = CreatePipelineShaderStageInfo();
			vertex_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertex_stage.module = vertex_shader;
			vertex_stage.pName = vertex_shader.GetEntrypointCStr();
			stages.emplace_back(vertex_stage);

			VkPipelineShaderStageCreateInfo fragment_stage = CreatePipelineShaderStageInfo();
			fragment_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragment_stage.module = fragment_shader;
			fragment_stage.pName = fragment_shader.GetEntrypointCStr();
			stages.emplace_back(fragment_stage);

			return stages;
		}

		static VkPipelineInputAssemblyStateCreateInfo CreatePipelineInputAssemblyStateInfo()
		{
			VkPipelineInputAssemblyStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			info.primitiveRestartEnable = VK_FALSE;
			// it's possible to break up lines and triangles in the _STRIP topology modes by using a special index of
			// 0xFFFF or 0xFFFFFFFF.
			return info;
		}

		static VkViewport CreateViewport(mem::sptr<RenderContext> context)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (flt)render_context.GetExtent().width;
			viewport.height = (flt)render_context.GetExtent().height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			return viewport;
		}

		static VkRect2D CreateScissor(mem::sptr<RenderContext> context)
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*context);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = render_context.GetExtent();
			return scissor;
		}

		static VkPipelineViewportStateCreateInfo CreatePipelineViewportStateInfo()
		{
			VkPipelineViewportStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			return info;
		}

		static VkPipelineRasterizationStateCreateInfo CreatePipelineRasterizationStateInfo()
		{
			VkPipelineRasterizationStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

			/*
			If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes
			are clamped to them as opposed to discarding them. This is useful in some special cases like
			shadow maps. Using this requires enabling a GPU feature.
			*/
			info.depthClampEnable = VK_FALSE;

			info.rasterizerDiscardEnable = VK_FALSE;
			info.polygonMode = VK_POLYGON_MODE_FILL; // or VK_POLYGON_MODE_LINE or VK_POLYGON_MODE_POINT
			info.lineWidth = 1.0f; // any larger value required enabling wideLines GPU feature
			info.cullMode = VK_CULL_MODE_BACK_BIT;
			info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

			/*
			The rasterizer can alter the depth values by adding a constant value or biasing them based on
			a fragment's slope. This is sometimes used for shadow mapping, but we won't be using it.
			Just set depthBiasEnable to VK_FALSE.
			*/
			info.depthBiasEnable = VK_FALSE;
			info.depthBiasConstantFactor = 0.0f; // Optional
			info.depthBiasClamp = 0.0f; // Optional
			info.depthBiasSlopeFactor = 0.0f; // Optional
			return info;
		}

		static VkPipelineMultisampleStateCreateInfo CreatePipelineMultisampleStateInfo()
		{
			VkPipelineMultisampleStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			info.sampleShadingEnable = VK_FALSE;
			info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			info.minSampleShading = 1.0f; // Optional
			info.pSampleMask = nullptr; // Optional
			info.alphaToCoverageEnable = VK_FALSE; // Optional
			info.alphaToOneEnable = VK_FALSE; // Optional
			return info;
		}

		static VkPipelineColorBlendAttachmentState CreatePipelineColorBlendAttachmentState()
		{
			VkPipelineColorBlendAttachmentState state{};
			state.colorWriteMask =
				VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			state.blendEnable = VK_FALSE;
			state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			state.colorBlendOp = VK_BLEND_OP_ADD; // Optional
			state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
			state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
			state.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
			return state;

			/*
				TODO:
					The most common way to use color blending is to implement alpha blending, where we want
					the new color to be blended with the old color based on its opacity

					finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
					finalColor.a = newAlpha.a;

					colorBlendAttachment.blendEnable = VK_TRUE;
					colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
					colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
					colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
					colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
					colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
					colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
			*/
		}

		static VkPipelineColorBlendStateCreateInfo CreatePipelineColorBlendStateInfo()
		{
			VkPipelineColorBlendStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			info.logicOpEnable = VK_FALSE;
			info.logicOp = VK_LOGIC_OP_COPY; // Optional
			info.blendConstants[0] = 0.0f; // Optional
			info.blendConstants[1] = 0.0f; // Optional
			info.blendConstants[2] = 0.0f; // Optional
			info.blendConstants[3] = 0.0f; // Optional
			return info;
		}

		static VkPipelineDepthStencilStateCreateInfo CreatePipelineDepthStencilStateInfo()
		{
			VkPipelineDepthStencilStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			info.depthTestEnable = VK_TRUE;
			info.depthWriteEnable = VK_TRUE;
			info.depthCompareOp = VK_COMPARE_OP_LESS;
			info.depthBoundsTestEnable = VK_FALSE;
			info.minDepthBounds = 0.0f;
			info.maxDepthBounds = 1.0f;
			info.stencilTestEnable = VK_FALSE;
			return info;
		}

		static VkGraphicsPipelineCreateInfo CreateGraphicsPipelineInfo()
		{
			VkGraphicsPipelineCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			return info;
		}

		static VkPipeline CreatePipeline(RenderPipeline::Properties properties, VkPipelineLayout pipeline_layout)
		{
			VkPipeline pipeline = nullptr;

			if (pipeline_layout)
			{
				VulkanRenderPass& render_pass = (VulkanRenderPass&)(*properties.framebuffers->GetRenderPass());
				VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*render_pass.GetRenderContext()->GetRenderDevice());

				con::vector<VkVertexInputBindingDescription> vertex_binding_descs = VulkanVertex::BindingDescriptions();
				con::array<VkVertexInputAttributeDescription, 3> vertex_attribute_descs = VulkanVertex::AttributeDescriptions();

				VkPipelineVertexInputStateCreateInfo vertex_input_state_info = CreatePipelineVertexInputStateInfo();
				vertex_input_state_info.vertexBindingDescriptionCount = (ui32)vertex_binding_descs.size();
				vertex_input_state_info.pVertexBindingDescriptions = vertex_binding_descs.data();
				vertex_input_state_info.vertexAttributeDescriptionCount = (ui32)vertex_attribute_descs.size();
				vertex_input_state_info.pVertexAttributeDescriptions = vertex_attribute_descs.data();

				con::vector<VkPipelineShaderStageCreateInfo> shader_stages = CreateShaderStages(properties);
				VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info = CreatePipelineInputAssemblyStateInfo();

				// TODO: use dynamic viewport and scissor:
				// https://github.com/Overv/VulkanTutorial/commit/87803541171579165caa354120157a0cc6c8192f
				con::vector<VkViewport> viewports{ CreateViewport(render_pass.GetRenderContext()) };
				con::vector<VkRect2D> scissors{ CreateScissor(render_pass.GetRenderContext()) };

				VkPipelineViewportStateCreateInfo viewport_state_info = CreatePipelineViewportStateInfo();
				viewport_state_info.viewportCount = viewports.size();
				viewport_state_info.pViewports = viewports.data();
				viewport_state_info.scissorCount = scissors.size();
				viewport_state_info.pScissors = scissors.data();

				VkPipelineRasterizationStateCreateInfo rasterization_state_info = CreatePipelineRasterizationStateInfo();
				VkPipelineMultisampleStateCreateInfo multisample_state_info = CreatePipelineMultisampleStateInfo();

				con::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states = {
					CreatePipelineColorBlendAttachmentState() };
				VkPipelineColorBlendStateCreateInfo color_blend_state_info = CreatePipelineColorBlendStateInfo();
				color_blend_state_info.attachmentCount = color_blend_attachment_states.size();
				color_blend_state_info.pAttachments = color_blend_attachment_states.data();

				VkPipelineDepthStencilStateCreateInfo depth_stencil_state_info = CreatePipelineDepthStencilStateInfo();

				VkGraphicsPipelineCreateInfo pipeline_info = CreateGraphicsPipelineInfo();
				pipeline_info.stageCount = shader_stages.size();
				pipeline_info.pStages = shader_stages.data();
				pipeline_info.pVertexInputState = &vertex_input_state_info;
				pipeline_info.pInputAssemblyState = &input_assembly_state_info;
				pipeline_info.pViewportState = &viewport_state_info;
				pipeline_info.pRasterizationState = &rasterization_state_info;
				pipeline_info.pMultisampleState = &multisample_state_info;
				pipeline_info.pDepthStencilState = nullptr; // Optional
				pipeline_info.pColorBlendState = &color_blend_state_info;
				pipeline_info.pDynamicState = nullptr; // Optional
				pipeline_info.layout = pipeline_layout;
				pipeline_info.renderPass = render_pass;
				pipeline_info.subpass = 0;
				pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
				pipeline_info.basePipelineIndex = -1; // Optional
				pipeline_info.pDepthStencilState = &depth_stencil_state_info;

				if (vkCreateGraphicsPipelines(*render_device.GetLogicalDevice(), nullptr, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
					pipeline = nullptr;
			}

			return pipeline;
		}

		void UpdateMetaValueBuffer()
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*GetRenderContext());
			ui32 index = render_context.GetCurrentImageIndex();
			VkDeviceMemory device_memory = _meta_value_buffers[index]->GetDeviceMemory();
			void* data;
			vkMapMemory(*GetLogicalDevice(), device_memory, 0, sizeof(PipelineMetaValues), 0, &data);
			memcpy(data, &_meta_values[index], sizeof(PipelineMetaValues));
			vkUnmapMemory(*GetLogicalDevice(), device_memory);
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			VulkanRenderDevice& render_device = (VulkanRenderDevice&)(*_properties.framebuffers->GetRenderPass()->GetRenderContext()->GetRenderDevice());
			return render_device.GetLogicalDevice();
		}

		mem::sptr<RenderContext> GetRenderContext() const
		{
			return _properties.framebuffers->GetRenderPass()->GetRenderContext();
		}

		void Dispose()
		{
			VulkanLogicalDevice& vulkan_logical_device = *GetLogicalDevice();
			vkDestroyPipeline(vulkan_logical_device, _pipeline, nullptr);
			vkDestroyPipelineLayout(vulkan_logical_device, _pipeline_layout, nullptr);
			_meta_value_buffers.clear();
			_meta_values.clear();

			_pipeline = nullptr;
			_pipeline_layout = nullptr;
		}

	public:
		VulkanRenderPipeline(RenderPipeline::Properties& properties) : 
			RenderPipeline(properties),
			_descriptor_set_layout(CreateDescriptorSetLayout(GetServices(), GetLogicalDevice())),
			_descriptor_sets(CreateDescriptorSets(GetServices(), GetRenderContext(), _descriptor_set_layout)),
			_meta_value_buffers(CreateMetaValueBuffers(GetServices(), GetRenderContext())),
			_meta_value_descriptor_infos(CreateMetaValueDescriptorInfos(GetRenderContext(), _meta_value_buffers)),
			_meta_value_descriptor_writers(CreateMetaValueDescriptorWriters(GetRenderContext(), _meta_value_descriptor_infos)),
			_sampler(CreateSampler(GetServices(), GetRenderContext(), VulkanSampler::CreateInfo())),
			_pipeline_layout(CreatePipelineLayout(GetRenderContext(), _descriptor_set_layout)),
			_pipeline(CreatePipeline(_properties, _pipeline_layout)),
			_bind_pipeline(nullptr),
			_bind_descriptor_sets_command_slot(0),
			_bound_descriptor_sets(),
			_bind_descriptor_sets(nullptr)
		{
			VulkanRenderContext& vulkan_render_context = (VulkanRenderContext&)(*GetRenderContext());
			_meta_values.resize(vulkan_render_context.GetFramesInFlightCount());
		}

		~VulkanRenderPipeline()
		{
			vkDeviceWaitIdle(*GetLogicalDevice());
			Dispose();
		}

		void BindPipeline(mem::sptr<CommandStaging> staging)
		{
			_bind_pipeline = mem::create_sptr<VulkanCommandBindPipeline>(GetServices()->GetAllocator(), VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
			staging->Stage(_bind_pipeline);
		}

		void PrepareToBindDescriptorSets(mem::sptr<CommandStaging> staging)
		{
			_bind_descriptor_sets_command_slot = staging->GetStagedSlot();
		}

		void BindDescriptorSets(mem::sptr<CommandStaging> staging)
		{
			VulkanRenderContext& vulkan_render_context = (VulkanRenderContext&)(*GetRenderContext());
			ui32 current_image_index = vulkan_render_context.GetCurrentImageIndex();
			GetDescriptorSets()->SubmitWriter(_meta_value_descriptor_writers[current_image_index], current_image_index);
			_bound_descriptor_sets = { (*_descriptor_sets)[current_image_index] };

			_bind_descriptor_sets = mem::create_sptr<VulkanCommandBindDescriptorSets>(GetServices()->GetAllocator(),
				VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout, 0, (ui32)_bound_descriptor_sets.size(),
				_bound_descriptor_sets.empty() ? nullptr : _bound_descriptor_sets.data(), 0,
				nullptr);

			staging->Stage(_bind_descriptor_sets_command_slot, _bind_descriptor_sets);
		}

		VkPipelineLayout GetLayout() const
		{
			return _pipeline_layout;
		}

		mem::sptr<VulkanSampler> GetDefaultSampler() const
		{
			return _sampler;
		}

		mem::sptr<VulkanDescriptorSets> GetDescriptorSets() const
		{
			return _descriptor_sets;
		}

		void Rebuild()
		{
			mem::sptr<RenderContext> render_context = GetRenderContext();
			VulkanRenderContext& vulkan_render_context = (VulkanRenderContext&)(*render_context);

			_descriptor_sets->Rebuild(vulkan_render_context.GetFramesInFlightCount());

			// TODO: pretty sure we only need to rebuild ubo stuff?? Nah I think everything needs to be rebuilt....??

			Dispose();

			_meta_values.resize(vulkan_render_context.GetFramesInFlightCount());
			_meta_value_buffers = CreateMetaValueBuffers(GetServices(), render_context);
			_meta_value_descriptor_infos = CreateMetaValueDescriptorInfos(render_context, _meta_value_buffers);
			_meta_value_descriptor_writers = CreateMetaValueDescriptorWriters(render_context, _meta_value_descriptor_infos);
			_pipeline_layout = CreatePipelineLayout(render_context, _descriptor_set_layout);
			_pipeline = CreatePipeline(_properties, _pipeline_layout);
		}

		virtual PipelineMetaValues GetMetaValues() const override
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*GetRenderContext());
			return _meta_values[render_context.GetCurrentImageIndex()];
		}

		virtual void SetMetaValues(PipelineMetaValues meta_values) override
		{
			VulkanRenderContext& render_context = (VulkanRenderContext&)(*GetRenderContext());
			_meta_values[render_context.GetCurrentImageIndex()] = meta_values;
			UpdateMetaValueBuffer();
		}
	};
}

#endif /* NP_ENGINE_VULKAN_RENDER_PIPELINE_HPP */