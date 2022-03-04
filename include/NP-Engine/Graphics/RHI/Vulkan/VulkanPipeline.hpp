//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_PIPELINE_HPP
#define NP_ENGINE_VULKAN_PIPELINE_HPP

#include "NP-Engine/Filesystem/Filesystem.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Time/Time.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"
#include "NP-Engine/Vendor/GlfwInclude.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanShader.hpp"
#include "VulkanVertex.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanCommandPool.hpp"
#include "VulkanCommands.hpp"
#include "VulkanCommandBuffer.hpp"

// TODO: there might be some methods with zero references...

namespace np::graphics::rhi
{
	class VulkanPipeline
	{
	private:
		constexpr static siz MAX_FRAMES = 2;

		VulkanSwapchain _swapchain;
		container::vector<VulkanVertex> _vertices;
		VulkanBuffer* _vertex_buffer;
		container::vector<ui16> _indices;
		VulkanBuffer* _index_buffer;
		VulkanBuffer* _staging_buffer;
		container::vector<VulkanBuffer*> _uniform_buffers;
		time::SteadyTimestamp _start_timestamp;
		VulkanShader _vertex_shader;
		VulkanShader _fragment_shader;
		VkDescriptorSetLayout _descriptor_set_layout;
		VkDescriptorPool _descriptor_pool;
		container::vector<VkDescriptorSet> _descriptor_sets;
		VkRenderPass _render_pass;
		VkPipelineLayout _pipeline_layout;
		VkPipeline _pipeline;

		bl _rebuild_swapchain;

		container::vector<VkFramebuffer> _framebuffers;
		VulkanCommandPool _command_pool;
		container::vector<VulkanCommandBuffer> _command_buffers;

		siz _current_frame;

		container::vector<VkSemaphore> _image_available_semaphores;
		container::vector<VkSemaphore> _render_finished_semaphores;
		container::vector<VkFence> _fences;
		container::vector<VkFence> _image_fences;

		void SetRebuildSwapchain(bl rebuild_swapchain = true)
		{
			_rebuild_swapchain = rebuild_swapchain;
		}

		// TODO: WindowResizeCallback and WindowPositionCallback can be called very fast in succession - add a threshold for
		// when these actually draw so we don't get so bogged down with draw calls

		static void WindowResizeCallback(void* pipeline, ui32 width, ui32 height)
		{
			((VulkanPipeline*)pipeline)->SetRebuildSwapchain();
			((VulkanPipeline*)pipeline)->Draw();
		}

		static void WindowPositionCallback(void* pipeline, i32 x, i32 y)
		{
			((VulkanPipeline*)pipeline)->SetRebuildSwapchain();
			((VulkanPipeline*)pipeline)->Draw();
		}

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
			// TODO: it's possible to break up lines and triangles in the _STRIP topology modes by using a special index of
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
			info.polygonMode = VK_POLYGON_MODE_FILL; // TODO: or VK_POLYGON_MODE_LINE or VK_POLYGON_MODE_POINT
			info.lineWidth = 1.0f; // TODO: any larger value required enabling wideLines GPU feature
			info.cullMode = VK_CULL_MODE_BACK_BIT;
			info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

			/*
			//TODO:
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

		VkAttachmentDescription CreateAttachmentDescription()
		{
			VkAttachmentDescription desc{};
			desc.format = GetDevice().GetSurfaceFormat().format;
			desc.samples = VK_SAMPLE_COUNT_1_BIT; // TODO: multisampling
			// TODO: The loadOp and storeOp determine what to do with the data in the
			//	attachment before rendering and after rendering
			desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			// TODO: we don't do anything with the stencil buffer
			desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

			desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			desc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			return desc;
		}

		VkAttachmentReference CreateAttachmentReference()
		{
			VkAttachmentReference ref{};
			ref.attachment = 0;
			ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			return ref;
		}

		VkSubpassDescription CreateSubpassDescription()
		{
			VkSubpassDescription desc{};
			desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

			return desc;
		}

		VkRenderPassCreateInfo CreateRenderPassInfo()
		{
			VkRenderPassCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
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

		VkPipelineLayoutCreateInfo CreatePipelineLayoutInfo()
		{
			VkPipelineLayoutCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			info.setLayoutCount = 0; // Optional
			info.pSetLayouts = nullptr; // Optional
			info.pushConstantRangeCount = 0; // Optional
			info.pPushConstantRanges = nullptr; // Optional
			return info;
		}

		VkFramebufferCreateInfo CreateFramebufferInfo()
		{
			VkFramebufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			info.width = GetSwapchain().GetExtent().width;
			info.height = GetSwapchain().GetExtent().height;
			info.layers = 1;
			return info;
		}

		VkCommandBufferAllocateInfo CreateCommandBufferAllocateInfo()
		{
			VkCommandBufferAllocateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			info.commandPool = _command_pool;
			info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			return info;
		}

		VkCommandBufferBeginInfo CreateCommandBufferBeginInfo()
		{
			VkCommandBufferBeginInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			info.flags = 0; // Optional
			info.pInheritanceInfo = nullptr; // Optional
			return info;
		}

		VkRenderPassBeginInfo CreateRenderPassBeginInfo()
		{
			VkRenderPassBeginInfo info{};
			info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			info.renderPass = _render_pass;
			info.renderArea.offset = {0, 0};
			info.renderArea.extent = GetSwapchain().GetExtent();
			return info;
		}

		container::vector<VkClearValue> CreateClearValues()
		{
			container::vector<VkClearValue> values;

			VkClearValue clear_color = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
			values.emplace_back(clear_color);

			return values;
		}

		container::vector<VkSubpassDependency> CreateSubpassDependencies()
		{
			container::vector<VkSubpassDependency> dependencies{};

			VkSubpassDependency dependency{};
			dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
			dependency.dstSubpass = 0;
			dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.srcAccessMask = 0;
			dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

			dependencies.emplace_back(dependency);

			return dependencies;
		}

		VkDescriptorSetLayoutCreateInfo CreateDescriptorSetLayoutCreateInfo()
		{
			VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
			descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;

			return descriptor_set_layout_create_info;
		}

		VkDescriptorSetLayout CreateDescriptorSetLayout()
		{
			VkDescriptorSetLayout descriptor_set_layout = nullptr;

			VkDescriptorSetLayoutBinding descriptor_set_layout_binding{};
			descriptor_set_layout_binding.binding = 0;
			descriptor_set_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_set_layout_binding.descriptorCount = 1;

			descriptor_set_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			descriptor_set_layout_binding.pImmutableSamplers = nullptr; // TODO: optional

			container::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings{descriptor_set_layout_binding};

			VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = CreateDescriptorSetLayoutCreateInfo();
			descriptor_set_layout_create_info.bindingCount = (ui32)descriptor_set_layout_bindings.size();
			descriptor_set_layout_create_info.pBindings = descriptor_set_layout_bindings.data();

			if (vkCreateDescriptorSetLayout(GetDevice(), &descriptor_set_layout_create_info, nullptr, &descriptor_set_layout) !=
				VK_SUCCESS)
			{
				descriptor_set_layout = nullptr;
			}

			return descriptor_set_layout;
		}

		VkRenderPass CreateRenderPass()
		{
			VkRenderPass render_pass = nullptr;

			container::vector<VkAttachmentReference> attachment_references = {CreateAttachmentReference()};

			VkSubpassDescription subpass_description = CreateSubpassDescription();
			subpass_description.colorAttachmentCount = attachment_references.size();
			subpass_description.pColorAttachments = attachment_references.data();

			container::vector<VkSubpassDescription> subpass_descriptions = {subpass_description};

			container::vector<VkAttachmentDescription> attachment_descriptions = {CreateAttachmentDescription()};

			container::vector<VkSubpassDependency> subpass_dependencies = CreateSubpassDependencies();

			VkRenderPassCreateInfo render_pass_info = CreateRenderPassInfo();
			render_pass_info.attachmentCount = attachment_descriptions.size();
			render_pass_info.pAttachments = attachment_descriptions.data();
			render_pass_info.subpassCount = subpass_descriptions.size();
			render_pass_info.pSubpasses = subpass_descriptions.data();
			render_pass_info.dependencyCount = subpass_dependencies.size();
			render_pass_info.pDependencies = subpass_dependencies.data();

			if (vkCreateRenderPass(GetDevice(), &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
			{
				render_pass = nullptr;
			}

			return render_pass;
		}

		VkPipelineLayout CreatePipelineLayout()
		{
			VkPipelineLayout pipeline_layout = nullptr;

			container::vector<VkDescriptorSetLayout> set_layouts{_descriptor_set_layout};

			VkPipelineLayoutCreateInfo pipeline_layout_info = CreatePipelineLayoutInfo();
			pipeline_layout_info.setLayoutCount = (ui32)set_layouts.size();
			pipeline_layout_info.pSetLayouts = set_layouts.data();

			if (vkCreatePipelineLayout(GetDevice(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
			{
				pipeline_layout = nullptr;
			}

			return pipeline_layout;
		}

		VkPipeline CreatePipeline()
		{
			VkPipeline pipeline = nullptr;

			if (_pipeline_layout != nullptr && _render_pass != nullptr)
			{
				container::vector<VkVertexInputBindingDescription> vertex_binding_descs = VulkanVertex::BindingDescriptions();
				container::array<VkVertexInputAttributeDescription, 2> vertex_attribute_descs =
					VulkanVertex::AttributeDescriptions();

				VkPipelineVertexInputStateCreateInfo vertex_input_state_info = CreatePipelineVertexInputStateInfo();
				vertex_input_state_info.vertexBindingDescriptionCount = (ui32)vertex_binding_descs.size();
				vertex_input_state_info.pVertexBindingDescriptions = vertex_binding_descs.data();
				vertex_input_state_info.vertexAttributeDescriptionCount = (ui32)vertex_attribute_descs.size();
				vertex_input_state_info.pVertexAttributeDescriptions = vertex_attribute_descs.data();

				container::vector<VkPipelineShaderStageCreateInfo> shader_stages = CreateShaderStages();
				VkPipelineInputAssemblyStateCreateInfo input_assembly_state_info = CreatePipelineInputAssemblyStateInfo();

				container::vector<VkViewport> viewports = {CreateViewport()};
				container::vector<VkRect2D> scissors = {CreateScissor()};

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

				if (vkCreateGraphicsPipelines(GetDevice(), nullptr, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
				{
					pipeline = nullptr;
				}
			}

			return pipeline;
		}

		container::vector<VkFramebuffer> CreateFramebuffers()
		{
			container::vector<VkFramebuffer> framebuffers(GetSwapchain().GetImageViews().size());

			for (siz i = 0; i < GetSwapchain().GetImageViews().size(); i++)
			{
				container::vector<VkImageView> image_views = {GetSwapchain().GetImageViews()[i]};

				VkFramebufferCreateInfo framebuffer_info = CreateFramebufferInfo();
				framebuffer_info.renderPass = _render_pass;
				framebuffer_info.attachmentCount = image_views.size();
				framebuffer_info.pAttachments = image_views.data();

				if (vkCreateFramebuffer(GetDevice(), &framebuffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS)
				{
					framebuffers[i] = nullptr;
				}
			}

			return framebuffers;
		}

		container::vector<VulkanCommandBuffer> CreateCommandBuffers()
		{
			VkCommandBufferAllocateInfo command_buffer_allocate_info = CreateCommandBufferAllocateInfo();
			command_buffer_allocate_info.commandBufferCount = (ui32)_framebuffers.size();
			container::vector<VulkanCommandBuffer> command_buffers =
				_command_pool.AllocateCommandBuffers(command_buffer_allocate_info);

			// TODO: I think we can pull the following out of the loop for now...
			VkCommandBufferBeginInfo command_buffer_begin_info = CreateCommandBufferBeginInfo();
			container::vector<VkClearValue> clear_values = CreateClearValues();
			container::vector<VkBuffer> vertex_buffers{*_vertex_buffer};
			container::vector<VkDeviceSize> offsets{0};

			VkRenderPassBeginInfo render_pass_begin_info = CreateRenderPassBeginInfo();
			render_pass_begin_info.clearValueCount = clear_values.size();
			render_pass_begin_info.pClearValues = clear_values.data();

			VulkanCommandBeginRenderPass begin_render_pass(render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
			VulkanCommandBindPipeline bind_pipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
			VulkanCommandBindVertexBuffers bind_vertex_buffers(0, (ui32)vertex_buffers.size(), vertex_buffers.data(),
															   offsets.data());
			VulkanCommandBindIndexBuffer bind_index_buffer(*_index_buffer, 0, VK_INDEX_TYPE_UINT16);
			VulkanCommandBindDescriptorSets bind_descriptor_sets(VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline_layout, 0, 1,
																 nullptr, 0, nullptr);
			VulkanCommandDrawIndexed draw_indexed((ui32)_indices.size(), 1, 0, 0, 0);
			VulkanCommandEndRenderPass end_render_pass;

			for (siz i = 0; i < command_buffers.size(); i++)
			{
				if (command_buffers[i].Begin(command_buffer_begin_info) != VK_SUCCESS)
				{
					command_buffers.clear();
					break;
				}

				begin_render_pass.RenderPassBeginInfo.framebuffer = _framebuffers[i];
				bind_descriptor_sets.DescriptorSets = &_descriptor_sets[i];

				command_buffers[i].Add(begin_render_pass);
				command_buffers[i].Add(bind_pipeline);
				command_buffers[i].Add(bind_vertex_buffers);
				command_buffers[i].Add(bind_index_buffer);
				command_buffers[i].Add(bind_descriptor_sets);
				command_buffers[i].Add(draw_indexed);
				command_buffers[i].Add(end_render_pass);

				if (command_buffers[i].End() != VK_SUCCESS)
				{
					command_buffers.clear();
					break;
				}
			}

			return command_buffers;
		}

		container::vector<VkSemaphore> CreateSemaphores(siz count)
		{
			container::vector<VkSemaphore> semaphores(count);

			for (siz i = 0; i < count; i++)
			{
				VkSemaphoreCreateInfo semaphore_info{};
				semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
				if (vkCreateSemaphore(GetDevice(), &semaphore_info, nullptr, &semaphores[i]) != VK_SUCCESS)
				{
					semaphores.clear();
					break;
				}
			}

			return semaphores;
		}

		container::vector<VkFence> CreateFences(siz count, bl initialize = true)
		{
			container::vector<VkFence> fences(count, nullptr);

			if (initialize)
			{
				for (siz i = 0; i < count; i++)
				{
					VkFenceCreateInfo fence_info{};
					fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
					fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

					if (vkCreateFence(GetDevice(), &fence_info, nullptr, &fences[i]) != VK_SUCCESS)
					{
						fences.clear();
						break;
					}
				}
			}

			return fences;
		}

		VulkanBuffer* CreateBuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage_flags,
								   VkMemoryPropertyFlags memory_property_flags)
		{
			memory::Block block = memory::DefaultTraitAllocator.Allocate(sizeof(VulkanBuffer));
			memory::Construct<VulkanBuffer>(block, GetDevice(), size, buffer_usage_flags, memory_property_flags);
			return (VulkanBuffer*)block.ptr;
		}

		void DestroyBuffer(VulkanBuffer* buffer)
		{
			memory::Destruct(buffer);
			memory::DefaultTraitAllocator.Deallocate(buffer);
		}

		void CopyBuffer(VulkanBuffer& dst, VulkanBuffer& src, VkDeviceSize size)
		{
			VkCommandBufferAllocateInfo command_buffer_allocate_info = CreateCommandBufferAllocateInfo();
			command_buffer_allocate_info.commandBufferCount = 1;
			container::vector<VulkanCommandBuffer> command_buffers =
				_command_pool.AllocateCommandBuffers(command_buffer_allocate_info);
			container::vector<VkCommandBuffer> buffers(command_buffers.begin(), command_buffers.end());

			VkCommandBufferBeginInfo command_buffer_begin_info = CreateCommandBufferBeginInfo();
			command_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
			vkBeginCommandBuffer(command_buffers.front(), &command_buffer_begin_info);

			container::vector<VkBufferCopy> buffer_copies{{}};
			buffer_copies.front().dstOffset = 0; // TODO: optional
			buffer_copies.front().srcOffset = 0; // TODO: optional
			buffer_copies.front().size = size;
			vkCmdCopyBuffer(command_buffers.front(), src, dst, (ui32)buffer_copies.size(), buffer_copies.data());

			vkEndCommandBuffer(command_buffers.front());

			container::vector<VkSubmitInfo> submit_infos{{}};
			submit_infos.front().sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_infos.front().pCommandBuffers = buffers.data();
			submit_infos.front().commandBufferCount = (ui32)buffers.size();

			vkQueueSubmit(GetDevice().GetGraphicsDeviceQueue(), submit_infos.size(), submit_infos.data(), nullptr);
			vkQueueWaitIdle(GetDevice().GetGraphicsDeviceQueue()); // TODO: use a fence so we can submit multiple simultaneously
																   // - this makes it one by one

			_command_pool.FreeCommandBuffers(command_buffers);
		}

		container::vector<VulkanBuffer*> CreateUniformBuffers() // TODO: return vector of ubo's
		{
			container::vector<VulkanBuffer*> uniform_buffers;

			for (siz i = 0; i < (ui32)GetSwapchain().GetImageViews().size(); i++)
			{
				uniform_buffers.emplace_back(
					CreateBuffer(sizeof(UniformBufferObject), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
								 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
			}

			return uniform_buffers;
		}

		void UpdateUniformBuffer(ui32 current_image)
		{
			time::DurationMilliseconds duration = time::SteadyClock::now() - _start_timestamp;
			flt seconds = duration.count() / 1000.0f;

			UniformBufferObject ubo{};
			ubo.Model = glm::rotate(glm::mat4(1.0f), seconds * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.Projection =
				glm::perspective(glm::radians(45.0f),
								 (flt)GetSwapchain().GetExtent().width / (flt)GetSwapchain().GetExtent().height, 0.1f, 10.0f);
			ubo.Projection[1][1] *= -1; // glm was made for OpenGL, so Y is inverted. We fix/invert Y here.

			void* data;
			vkMapMemory(GetDevice(), _uniform_buffers[current_image]->GetDeviceMemory(), 0, sizeof(UniformBufferObject), 0,
						&data);
			memcpy(data, &ubo, sizeof(UniformBufferObject));
			vkUnmapMemory(GetDevice(), _uniform_buffers[current_image]->GetDeviceMemory());

			// TODO: using UBO's this way is not efficient - we should use push constants
		}

		void DestroyUniformBuffers() // TODO: I don't know if we need this...
		{
			for (auto it = _uniform_buffers.begin(); it != _uniform_buffers.end(); it++)
				DestroyBuffer(*it);
		}

		VkDescriptorPool CreateDescriptorPool()
		{
			VkDescriptorPool descriptor_pool = nullptr;

			container::vector<VkDescriptorPoolSize> descriptor_pool_sizes{{}}; // TODO: have method create this?
			descriptor_pool_sizes.front().type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptor_pool_sizes.front().descriptorCount = (ui32)GetSwapchain().GetImageViews().size();

			VkDescriptorPoolCreateInfo descriptor_pool_create_info{}; // TODO: have method create this?
			descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			descriptor_pool_create_info.poolSizeCount = (ui32)descriptor_pool_sizes.size();
			descriptor_pool_create_info.pPoolSizes = descriptor_pool_sizes.data();
			descriptor_pool_create_info.maxSets = (ui32)GetSwapchain().GetImageViews().size();

			if (vkCreateDescriptorPool(GetDevice(), &descriptor_pool_create_info, nullptr, &descriptor_pool) != VK_SUCCESS)
			{
				descriptor_pool = nullptr;
			}

			return descriptor_pool;
		}

		void DestroyDescriptorPool()
		{
			vkDestroyDescriptorPool(GetDevice(), _descriptor_pool, nullptr);
		}

		container::vector<VkDescriptorSet> CreateDescriptorSets()
		{
			container::vector<VkDescriptorSet> descriptor_sets;

			container::vector<VkDescriptorSetLayout> descriptor_set_layouts((ui32)GetSwapchain().GetImageViews().size(),
																			_descriptor_set_layout);

			VkDescriptorSetAllocateInfo descriptor_set_allocate_info{}; // TODO: make method create this?
			descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptor_set_allocate_info.descriptorPool = _descriptor_pool;
			descriptor_set_allocate_info.descriptorSetCount = (ui32)descriptor_set_layouts.size();
			descriptor_set_allocate_info.pSetLayouts = descriptor_set_layouts.data();

			descriptor_sets.resize((ui32)GetSwapchain().GetImageViews().size());
			if (vkAllocateDescriptorSets(GetDevice(), &descriptor_set_allocate_info, descriptor_sets.data()) != VK_SUCCESS)
			{
				descriptor_sets.clear();
			}

			for (siz i = 0; i < descriptor_sets.size(); i++)
			{
				VkDescriptorBufferInfo descriptor_buffer_info{}; // TODO: have method create this?
				descriptor_buffer_info.buffer = *_uniform_buffers[i];
				descriptor_buffer_info.offset = 0;
				descriptor_buffer_info.range = sizeof(UniformBufferObject);

				VkWriteDescriptorSet write_descriptor_set{}; // TODO: have method create this?
				write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write_descriptor_set.dstSet = descriptor_sets[i];
				write_descriptor_set.dstBinding = 0;
				write_descriptor_set.dstArrayElement = 0;
				write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				write_descriptor_set.descriptorCount = 1;
				write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
				write_descriptor_set.pImageInfo = nullptr; // TODO: optional
				write_descriptor_set.pTexelBufferView = nullptr; // TODO: optional

				vkUpdateDescriptorSets(GetDevice(), 1, &write_descriptor_set, 0, nullptr);
			}

			return descriptor_sets;
		}

		void RebuildSwapchain()
		{
			SetRebuildSwapchain(false);

			vkDeviceWaitIdle(GetDevice());

			_command_pool.FreeCommandBuffers(_command_buffers);

			DestroyDescriptorPool();
			DestroyUniformBuffers();

			for (auto framebuffer : _framebuffers)
				vkDestroyFramebuffer(GetDevice(), framebuffer, nullptr);

			vkDestroyPipeline(GetDevice(), _pipeline, nullptr);
			vkDestroyPipelineLayout(GetDevice(), _pipeline_layout, nullptr);
			vkDestroyRenderPass(GetDevice(), _render_pass, nullptr);

			_swapchain.Rebuild();

			_render_pass = CreateRenderPass();
			_pipeline_layout = CreatePipelineLayout();
			_pipeline = CreatePipeline();
			_framebuffers = CreateFramebuffers();
			_uniform_buffers = CreateUniformBuffers();
			_descriptor_pool = CreateDescriptorPool();
			_descriptor_sets = CreateDescriptorSets();
			_command_buffers = CreateCommandBuffers();
		}

	public:
		VulkanPipeline(VulkanDevice& device):
			_swapchain(device),
			_vertex_buffer(nullptr),
			_start_timestamp(time::SteadyClock::now()),
			_vertex_shader(GetDevice(), fs::Append(fs::Append("Vulkan", "shaders"), "vertex.glsl"), VulkanShader::Type::VERTEX),
			_fragment_shader(GetDevice(), fs::Append(fs::Append("Vulkan", "shaders"), "fragment.glsl"),
							 VulkanShader::Type::FRAGMENT),
			_descriptor_set_layout(CreateDescriptorSetLayout()),
			_render_pass(CreateRenderPass()),
			_pipeline_layout(CreatePipelineLayout()),
			_pipeline(CreatePipeline()),
			_framebuffers(CreateFramebuffers()), // TODO: can we put this in a VulkanFramebuffer? I don't think so
			_command_pool(device),
			_current_frame(0),
			_image_available_semaphores(CreateSemaphores(MAX_FRAMES)),
			_render_finished_semaphores(CreateSemaphores(MAX_FRAMES)),
			_fences(CreateFences(MAX_FRAMES)),
			_image_fences(CreateFences(GetSwapchain().GetImages().size(), false)),
			_rebuild_swapchain(false)
		{
			GetSurface().GetWindow().SetResizeCallback(this, WindowResizeCallback);
			GetSurface().GetWindow().SetPositionCallback(this, WindowPositionCallback);

			// vertex buffer
			_vertices = {{{{-0.5f, -0.5f}, {1.0f, 0.0f, 1.0f}}},
						 {{{0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}},
						 {{{0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}}},
						 {{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}}};

			VkDeviceSize data_size = sizeof(_vertices[0]) * _vertices.size();

			_staging_buffer = CreateBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
										   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			_vertex_buffer = CreateBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
										  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			void* data;
			vkMapMemory(device, _staging_buffer->GetDeviceMemory(), 0, data_size, 0, &data);
			memcpy(data, _vertices.data(), data_size);
			vkUnmapMemory(device, _staging_buffer->GetDeviceMemory());

			CopyBuffer(*_vertex_buffer, *_staging_buffer, data_size);
			DestroyBuffer(_staging_buffer);

			// index buffer
			_indices = {0, 1, 2, 2, 3, 0};

			data_size = sizeof(_indices[0]) * _indices.size();

			_staging_buffer = CreateBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
										   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			_index_buffer = CreateBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
										 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			vkMapMemory(device, _staging_buffer->GetDeviceMemory(), 0, data_size, 0, &data);
			memcpy(data, _indices.data(), data_size);
			vkUnmapMemory(device, _staging_buffer->GetDeviceMemory());

			CopyBuffer(*_index_buffer, *_staging_buffer, data_size);
			DestroyBuffer(_staging_buffer);

			_uniform_buffers = CreateUniformBuffers();
			_descriptor_pool = CreateDescriptorPool();
			_descriptor_sets = CreateDescriptorSets();

			_command_buffers = CreateCommandBuffers();
		}

		~VulkanPipeline()
		{
			vkDeviceWaitIdle(GetDevice());

			DestroyBuffer(_vertex_buffer);
			DestroyBuffer(_index_buffer);

			for (VkSemaphore& semaphore : _render_finished_semaphores)
				vkDestroySemaphore(GetDevice(), semaphore, nullptr);

			for (VkSemaphore& semaphore : _image_available_semaphores)
				vkDestroySemaphore(GetDevice(), semaphore, nullptr);

			for (VkFence& fence : _fences)
				vkDestroyFence(GetDevice(), fence, nullptr);

			DestroyDescriptorPool();
			DestroyUniformBuffers();

			for (auto framebuffer : _framebuffers)
				vkDestroyFramebuffer(GetDevice(), framebuffer, nullptr);

			vkDestroyPipeline(GetDevice(), _pipeline, nullptr);
			vkDestroyPipelineLayout(GetDevice(), _pipeline_layout, nullptr);
			vkDestroyRenderPass(GetDevice(), _render_pass, nullptr);
			vkDestroyDescriptorSetLayout(GetDevice(), _descriptor_set_layout, nullptr);
		}

		VulkanInstance& GetInstance() const
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

		void Draw()
		{
			i32 width = 0;
			i32 height = 0;
			glfwGetFramebufferSize((GLFWwindow*)GetSurface().GetWindow().GetNativeWindow(), &width, &height);
			if (width == 0 || height == 0)
			{
				return;
			}

			vkWaitForFences(GetDevice(), 1, &_fences[_current_frame], VK_TRUE, UI64_MAX);

			ui32 image_index;
			VkResult acquire_result = vkAcquireNextImageKHR(GetDevice(), GetSwapchain(), UI64_MAX,
															_image_available_semaphores[_current_frame], nullptr, &image_index);

			if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				RebuildSwapchain();
				return;
			}
			else if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR)
			{
				NP_ASSERT(false, "vkAcquireNextImageKHR error");
			}

			// Check if a previous frame is using this image (i.e. there is its fence to wait on)
			if (_image_fences[image_index] != nullptr)
			{
				vkWaitForFences(GetDevice(), 1, &_image_fences[image_index], VK_TRUE, UI64_MAX);
			}
			// Mark the image as now being in use by this frame
			_image_fences[image_index] = _fences[_current_frame];

			container::vector<VkSemaphore> wait_semaphores{_image_available_semaphores[_current_frame]};
			container::vector<VkPipelineStageFlags> wait_stages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			container::vector<VkSemaphore> signal_semaphores{_render_finished_semaphores[_current_frame]};
			container::vector<VkSwapchainKHR> swapchains{GetSwapchain()};

			UpdateUniformBuffer(image_index);

			VkCommandBuffer buffer = _command_buffers[image_index];

			VkSubmitInfo submit_info{};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.waitSemaphoreCount = wait_semaphores.size();
			submit_info.pWaitSemaphores = wait_semaphores.data();
			submit_info.pWaitDstStageMask = wait_stages.data();
			submit_info.commandBufferCount = 1; // _command_buffers.size();
			submit_info.pCommandBuffers = &buffer;
			submit_info.signalSemaphoreCount = signal_semaphores.size();
			submit_info.pSignalSemaphores = signal_semaphores.data();

			vkResetFences(GetDevice(), 1, &_fences[_current_frame]);

			if (vkQueueSubmit(GetDevice().GetGraphicsDeviceQueue(), 1, &submit_info, _fences[_current_frame]) != VK_SUCCESS)
			{
				NP_ASSERT(false, "failed to submit draw command buffer!");
			}

			VkPresentInfoKHR present_info{};
			present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present_info.waitSemaphoreCount = signal_semaphores.size();
			present_info.pWaitSemaphores = signal_semaphores.data();
			present_info.swapchainCount = swapchains.size();
			present_info.pSwapchains = swapchains.data();
			present_info.pImageIndices = &image_index;
			present_info.pResults = nullptr; // Optional

			VkResult present_result = vkQueuePresentKHR(GetDevice().GetPresentDeviceQueue(), &present_info);

			if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR || _rebuild_swapchain)
			{
				RebuildSwapchain();
			}
			else if (present_result != VK_SUCCESS)
			{
				NP_ASSERT(false, "vkQueuePresentKHR error");
			}

			vkQueueWaitIdle(GetDevice().GetPresentDeviceQueue());

			_current_frame = (_current_frame + 1) % MAX_FRAMES;
		}

		operator VkPipeline() const
		{
			return _pipeline;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_PIPELINE_HPP */