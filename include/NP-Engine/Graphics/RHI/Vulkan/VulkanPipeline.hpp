//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_PIPELINE_HPP
#define NP_ENGINE_VULKAN_PIPELINE_HPP

#include <iostream> //TODO: remove
#include <utility> //TODO: may not need...

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Filesystem/Filesystem.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"
#include "NP-Engine/Vendor/GlfwInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanSwapchain.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanShader.hpp"
#include "VulkanDescriptorSets.hpp"
#include "VulkanDescriptorSetLayout.hpp"

#include "VulkanVertex.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"
#include "VulkanImageView.hpp"
#include "VulkanSampler.hpp"
#include "VulkanTexture.hpp"
#include "VulkanFrame.hpp"

// TODO: there might be some methods with zero references...
// TODO: remove Optional comments

namespace np::graphics::rhi
{
	class VulkanPipeline : public Pipeline
	{
	private:
		VulkanSwapchain& _swapchain;
		VulkanRenderPass& _render_pass;
		VulkanShader& _vertex_shader;
		VulkanShader& _fragment_shader;

		VulkanDescriptorSetLayout _descriptor_set_layout;
		VulkanDescriptorSets _descriptor_sets;
		container::vector<VulkanBuffer*> _meta_value_buffers;
		container::vector<VkDescriptorBufferInfo> _meta_value_descriptor_infos;
		container::vector<VkWriteDescriptorSet> _meta_value_descriptor_writers;

		VulkanSampler _sampler; // TODO: we might should put this in the Renderer
		VkPipelineLayout _pipeline_layout;
		VkPipeline _pipeline;
		VulkanCommandBindPipeline* _bind_pipeline;
		siz _bind_descriptor_sets_command_slot;
		container::vector<VkDescriptorSet> _bound_descriptor_sets;
		VulkanCommandBindDescriptorSets* _bind_descriptor_sets;

		VkPipelineShaderStageCreateInfo CreatePipelineShaderStageInfo()
		{
			VkPipelineShaderStageCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			return info;
		}

		VkPipelineVertexInputStateCreateInfo CreatePipelineVertexInputStateInfo()
		{
			VkPipelineVertexInputStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			return info;
		}

		VkPipelineInputAssemblyStateCreateInfo CreatePipelineInputAssemblyStateInfo()
		{
			VkPipelineInputAssemblyStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			info.primitiveRestartEnable = VK_FALSE;
			// it's possible to break up lines and triangles in the _STRIP topology modes by using a special index of
			// 0xFFFF or 0xFFFFFFFF.
			return info;
		}

		VkViewport CreateViewport()
		{
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (flt)GetSwapchain().GetExtent().width;
			viewport.height = (flt)GetSwapchain().GetExtent().height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			return viewport;
		}

		VkRect2D CreateScissor()
		{
			VkRect2D scissor{};
			scissor.offset = {0, 0};
			scissor.extent = GetSwapchain().GetExtent();
			return scissor;
		}

		VkPipelineViewportStateCreateInfo CreatePipelineViewportStateInfo()
		{
			VkPipelineViewportStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			return info;
		}

		VkPipelineRasterizationStateCreateInfo CreatePipelineRasterizationStateInfo()
		{
			VkPipelineRasterizationStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

			/*
			//TODO:
			If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes
			are clamped to them as opposed to discarding them. This is useful in some special cases like
			shadow maps. Using this requires enabling a GPU feature.
			*/
			info.depthClampEnable = VK_FALSE;

			info.rasterizerDiscardEnable = VK_FALSE;
			info.polygonMode = VK_POLYGON_MODE_FILL; // or VK_POLYGON_MODE_LINE or VK_POLYGON_MODE_POINT
			info.lineWidth = 1.0f; // TODO: any larger value required enabling wideLines GPU feature
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

		VkPipelineMultisampleStateCreateInfo CreatePipelineMultisampleStateInfo()
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

		VkPipelineColorBlendAttachmentState CreatePipelineColorBlendAttachmentState()
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
			//TODO:
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

		VkPipelineColorBlendStateCreateInfo CreatePipelineColorBlendStateInfo()
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

		VkPipelineDepthStencilStateCreateInfo CreatePipelineDepthStencilStateInfo()
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

		// TODO: what's going on with these dynamic state methods??

		container::vector<VkDynamicState> CreateDynamicStates()
		{
			return {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_LINE_WIDTH};
		}

		VkPipelineDynamicStateCreateInfo CreatePipelineDynamicStateInfo()
		{
			VkPipelineDynamicStateCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			info.dynamicStateCount = 2;
			// info.pDynamicStates = dynamicStates;
			return info;
		}

		VkGraphicsPipelineCreateInfo CreateGraphicsPipelineInfo()
		{
			VkGraphicsPipelineCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			return info;
		}

		container::vector<VkPipelineShaderStageCreateInfo> CreateShaderStages()
		{
			container::vector<VkPipelineShaderStageCreateInfo> stages;

			VkPipelineShaderStageCreateInfo vertex_stage = CreatePipelineShaderStageInfo();
			vertex_stage.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertex_stage.module = _vertex_shader;
			vertex_stage.pName = _vertex_shader.GetEntrypoint().c_str();
			stages.emplace_back(vertex_stage);

			VkPipelineShaderStageCreateInfo fragment_stage = CreatePipelineShaderStageInfo();
			fragment_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragment_stage.module = _fragment_shader;
			fragment_stage.pName = _fragment_shader.GetEntrypoint().c_str();
			stages.emplace_back(fragment_stage);

			return stages;
		}

		VkPushConstantRange CreatePushConstantRange()
		{
			VkPushConstantRange range{};
			range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
			range.offset = 0;
			range.size = sizeof(RenderableMetaValues);
			return range;
		}

		VkPipelineLayoutCreateInfo CreatePipelineLayoutInfo()
		{
			VkPipelineLayoutCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			info.setLayoutCount = 0; // Optional
			info.pSetLayouts = nullptr; // Optional
			return info;
		}

		VkPipelineLayout CreatePipelineLayout()
		{
			VkPipelineLayout layout = nullptr;
			VkDescriptorSetLayout descriptor_layout = _descriptor_set_layout;
			VkPushConstantRange push_constant_range = CreatePushConstantRange();
			VkPipelineLayoutCreateInfo create_info = CreatePipelineLayoutInfo();
			// TODO: setLayoutCount must be less than or equal to VkPhysicalDeviceLimits::maxBoundDescriptorSets
			create_info.setLayoutCount = 1;
			create_info.pSetLayouts = &descriptor_layout;
			create_info.pushConstantRangeCount = 1;
			create_info.pPushConstantRanges = &push_constant_range;

			if (vkCreatePipelineLayout(GetDevice(), &create_info, nullptr, &layout) != VK_SUCCESS)
				layout = nullptr;

			return layout;
		}

		VkPipeline CreatePipeline()
		{
			VkPipeline pipeline = nullptr;

			if (_pipeline_layout != nullptr && _render_pass != nullptr)
			{
				container::vector<VkVertexInputBindingDescription> vertex_binding_descs = VulkanVertex::BindingDescriptions();
				container::array<VkVertexInputAttributeDescription, 3> vertex_attribute_descs =
					VulkanVertex::AttributeDescriptions();

				VkPipelineVertexInputStateCreateInfo vertex_input_state_info = CreatePipelineVertexInputStateInfo();
				vertex_input_state_info.vertexBindingDescriptionCount = (ui32)vertex_binding_descs.size();
				vertex_input_state_info.pVertexBindingDescriptions = vertex_binding_descs.data();
				vertex_input_state_info.vertexAttributeDescriptionCount = (ui32)vertex_attribute_descs.size();
				vertex_input_state_info.pVertexAttributeDescriptions = vertex_attribute_descs.data();

				container::vector<VkPipelineShaderStageCreateInfo> shader_stages = CreateShaderStages();
				VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info = CreatePipelineInputAssemblyStateInfo();

				container::vector<VkViewport> viewports{CreateViewport()};
				container::vector<VkRect2D> scissors{CreateScissor()};

				VkPipelineViewportStateCreateInfo viewport_state_info = CreatePipelineViewportStateInfo();
				viewport_state_info.viewportCount = viewports.size();
				viewport_state_info.pViewports = viewports.data();
				viewport_state_info.scissorCount = scissors.size();
				viewport_state_info.pScissors = scissors.data();

				VkPipelineRasterizationStateCreateInfo rasterization_state_info = CreatePipelineRasterizationStateInfo();
				VkPipelineMultisampleStateCreateInfo multisample_state_info = CreatePipelineMultisampleStateInfo();

				container::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states = {
					CreatePipelineColorBlendAttachmentState()};
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
				pipeline_info.layout = _pipeline_layout;
				pipeline_info.renderPass = _render_pass;
				pipeline_info.subpass = 0;
				pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
				pipeline_info.basePipelineIndex = -1; // Optional
				pipeline_info.pDepthStencilState = &depth_stencil_state_info;

				if (vkCreateGraphicsPipelines(GetDevice(), nullptr, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
				{
					pipeline = nullptr;
				}
			}

			return pipeline;
		}

		VulkanSampler* CreateSampler(VkSamplerCreateInfo& info)
		{
			return memory::Create<VulkanSampler>(memory::DefaultTraitAllocator, GetDevice(), info);
		}

		VulkanImage* CreateImage(VkImageCreateInfo& image_create_info, VkMemoryPropertyFlags memory_property_flags)
		{
			return memory::Create<VulkanImage>(memory::DefaultTraitAllocator, GetDevice(), image_create_info,
											   memory_property_flags);
		}

		VulkanBuffer* CreateBuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage_flags,
								   VkMemoryPropertyFlags memory_property_flags)
		{
			VkBufferCreateInfo info = VulkanBuffer::CreateInfo();
			info.size = size;
			info.usage = buffer_usage_flags;

			return memory::Create<VulkanBuffer>(memory::DefaultTraitAllocator, GetDevice(), info, memory_property_flags);
		}

		void UpdateMetaValueBuffer()
		{
			ui32 index = GetSwapchain().GetCurrentImageIndex();
			VkDeviceMemory device_memory = _meta_value_buffers[index]->GetDeviceMemory();
			void* data;
			vkMapMemory(GetDevice(), device_memory, 0, sizeof(PipelineMetaValues), 0, &data);
			memcpy(data, &_meta_values[index], sizeof(PipelineMetaValues));
			vkUnmapMemory(GetDevice(), device_memory);
		}

		container::vector<VulkanBuffer*> CreateMetaValueBuffers()
		{
			container::vector<VulkanBuffer*> buffers(GetSwapchain().GetImages().size());

			for (siz i = 0; i < buffers.size(); i++)
			{
				buffers[i] = CreateBuffer(sizeof(PipelineMetaValues), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
										  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			}

			return buffers;
		}

		container::vector<VkDescriptorBufferInfo> CreateMetaValueDescriptorInfos()
		{
			container::vector<VkDescriptorBufferInfo> infos(GetSwapchain().GetImages().size());

			for (siz i = 0; i < infos.size(); i++)
			{
				infos[i].buffer = *_meta_value_buffers[i];
				infos[i].offset = 0;
				infos[i].range = sizeof(PipelineMetaValues);
			}

			return infos;
		}

		container::vector<VkWriteDescriptorSet> CreateMetaValueDescriptorWriters()
		{
			container::vector<VkWriteDescriptorSet> writers(GetSwapchain().GetImages().size(),
															{VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET});

			for (siz i = 0; i < writers.size(); i++)
			{
				writers[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				writers[i].pBufferInfo = memory::AddressOf(_meta_value_descriptor_infos[i]);
				writers[i].descriptorCount = 1;
			}

			return writers;
		}

		void DestroyMetaValueBuffers()
		{
			for (VulkanBuffer*& meta_value_buffer : _meta_value_buffers)
				memory::Destroy<VulkanBuffer>(memory::DefaultTraitAllocator, meta_value_buffer);
		}

	public:
		VulkanPipeline(VulkanSwapchain& swapchain, VulkanRenderPass& render_pass, VulkanShader& vertex_shader,
					   VulkanShader& fragment_shader):
			_swapchain(swapchain),
			_render_pass(render_pass),
			_vertex_shader(vertex_shader),
			_fragment_shader(fragment_shader),
			_descriptor_set_layout(swapchain.GetDevice()),
			_descriptor_sets(swapchain, _descriptor_set_layout),
			_meta_value_buffers(CreateMetaValueBuffers()),
			_meta_value_descriptor_infos(CreateMetaValueDescriptorInfos()),
			_meta_value_descriptor_writers(CreateMetaValueDescriptorWriters()),
			_sampler(swapchain.GetDevice(), VulkanSampler::CreateInfo()),
			_pipeline_layout(CreatePipelineLayout()),
			_pipeline(CreatePipeline()),
			_bind_pipeline(nullptr),
			_bind_descriptor_sets_command_slot(0),
			_bind_descriptor_sets(nullptr)
		{
			_meta_values.resize(GetSwapchain().GetImages().size());
		}

		~VulkanPipeline()
		{
			vkDestroyPipeline(GetDevice(), _pipeline, nullptr);
			vkDestroyPipelineLayout(GetDevice(), _pipeline_layout, nullptr);
			DestroyMetaValueBuffers();
		}

		operator VkPipeline() const
		{
			return _pipeline;
		}

		VulkanInstance& GetInstance()
		{
			return _swapchain.GetInstance();
		}

		const VulkanInstance& GetInstance() const
		{
			return _swapchain.GetInstance();
		}

		VulkanSurface& GetSurface() const
		{
			return _swapchain.GetSurface();
		}

		VulkanDevice& GetDevice() const
		{
			return _swapchain.GetDevice();
		}

		const VulkanSwapchain& GetSwapchain() const
		{
			return _swapchain;
		}

		const VulkanShader& GetVertexShader() const
		{
			return _vertex_shader;
		}

		const VulkanShader& GetFragmentShader() const
		{
			return _fragment_shader;
		}

		VkPipelineLayout GetPipelineLayout() const
		{
			return _pipeline_layout;
		}

		VulkanDescriptorSets& GetDescriptorSets()
		{
			return _descriptor_sets;
		}

		const VulkanDescriptorSets& GetDescriptorSets() const
		{
			return _descriptor_sets;
		}

		VulkanSampler& GetDefaultSampler()
		{
			return _sampler;
		}

		const VulkanSampler& GetDefaultSampler() const
		{
			return _sampler;
		}

		void BindPipelineToFrame(VulkanFrame& frame)
		{
			if (_bind_pipeline)
				memory::Destroy<VulkanCommandBindPipeline>(memory::DefaultTraitAllocator, _bind_pipeline);

			_bind_pipeline = memory::Create<VulkanCommandBindPipeline>(memory::DefaultTraitAllocator,
																	   VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
			frame.StageCommand(*_bind_pipeline);
		}

		void PrepareToBindDescriptorSets(VulkanFrame& frame)
		{
			_bind_descriptor_sets_command_slot = frame.GetStagedSlot();
		}

		void BindDescriptorSetsToFrame(VulkanFrame& frame)
		{
			GetDescriptorSets().SubmitWriter(_meta_value_descriptor_writers[GetSwapchain().GetCurrentImageIndex()]);

			if (_bind_descriptor_sets)
				memory::Destroy<VulkanCommandBindDescriptorSets>(memory::DefaultTraitAllocator, _bind_descriptor_sets);

			_bound_descriptor_sets = {_descriptor_sets[GetSwapchain().GetCurrentImageIndex()]};

			_bind_descriptor_sets = memory::Create<VulkanCommandBindDescriptorSets>(
				memory::DefaultTraitAllocator, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout, 0,
				(ui32)_bound_descriptor_sets.size(),
				_bound_descriptor_sets.empty() ? nullptr : const_cast<VkDescriptorSet*>(_bound_descriptor_sets.data()), 0,
				nullptr);

			frame.StageCommand(_bind_descriptor_sets_command_slot, *_bind_descriptor_sets);
		}

		PipelineMetaValues GetMetaValues() const override
		{
			return _meta_values[GetSwapchain().GetCurrentImageIndex()];
		}

		void SetMetaValues(PipelineMetaValues meta_values) override
		{
			_meta_values[GetSwapchain().GetCurrentImageIndex()] = meta_values;
			UpdateMetaValueBuffer();
		}

		void Rebuild()
		{
			_descriptor_sets.Rebuild();

			// TODO: pretty sure we only need to rebuild ubo stuff?? Nah I think everything needs to be rebuilt....??

			vkDestroyPipeline(GetDevice(), _pipeline, nullptr);
			vkDestroyPipelineLayout(GetDevice(), _pipeline_layout, nullptr);
			DestroyMetaValueBuffers();
			_meta_values.resize(GetSwapchain().GetImages().size());
			_meta_value_buffers = CreateMetaValueBuffers();
			_meta_value_descriptor_infos = CreateMetaValueDescriptorInfos();
			_meta_value_descriptor_writers = CreateMetaValueDescriptorWriters();
			_pipeline_layout = CreatePipelineLayout();
			_pipeline = CreatePipeline();
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_PIPELINE_HPP */