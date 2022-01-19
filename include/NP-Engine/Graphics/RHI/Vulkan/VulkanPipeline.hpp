//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/8/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_PIPELINE_HPP
#define NP_ENGINE_VULKAN_PIPELINE_HPP

#include "NP-Engine/Filesystem/Filesystem.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"
#include "NP-Engine/Vendor/GlfwInclude.hpp"

#include "VulkanInstance.hpp"
#include "VulkanSurface.hpp"
#include "VulkanDevice.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanShader.hpp"
#include "VulkanVertex.hpp"

namespace np::graphics::rhi
{
	class VulkanPipeline
	{
	private:
		constexpr static siz MAX_FRAMES = 2;

		VulkanSwapchain _swapchain;
		container::vector<VulkanVertex> _vertices;
		VkBuffer _vertex_buffer;
		VkDeviceMemory _vertex_buffer_memory;
		VulkanShader _vertex_shader;
		VulkanShader _fragment_shader;
		VkRenderPass _render_pass;
		VkPipelineLayout _pipeline_layout;
		VkPipeline _pipeline;

		bl _rebuild_swapchain;

		container::vector<VkFramebuffer> _framebuffers;
		VkCommandPool _command_pool;
		container::vector<VkCommandBuffer> _command_buffers;

		siz _current_frame;

		container::vector<VkSemaphore> _image_available_semaphores;
		container::vector<VkSemaphore> _render_finished_semaphores;
		container::vector<VkFence> _fences;
		container::vector<VkFence> _image_fences;

		void SetRebuildSwapchain(bl rebuild_swapchain = true)
		{
			_rebuild_swapchain = rebuild_swapchain;
		}

		static void WindowResizeCallback(void* pipeline, ui32 width, ui32 height)
		{
			((VulkanPipeline*)pipeline)->SetRebuildSwapchain();
			((VulkanPipeline*)pipeline)->Draw(time::DurationMilliseconds(0));
		}

		static void WindowPositionCallback(void* pipeline, i32 x, i32 y)
		{
			((VulkanPipeline*)pipeline)->SetRebuildSwapchain();
			((VulkanPipeline*)pipeline)->Draw(time::DurationMilliseconds(0));
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
			viewport.width = (flt)Swapchain().Extent().width;
			viewport.height = (flt)Swapchain().Extent().height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			return viewport;
		}

		VkRect2D CreateScissor()
		{
			VkRect2D scissor{};
			scissor.offset = {0, 0};
			scissor.extent = Swapchain().Extent();
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
			info.frontFace = VK_FRONT_FACE_CLOCKWISE;

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
			desc.format = Device().SurfaceFormat().format;
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
			vertex_stage.pName = _vertex_shader.Entrypoint().c_str();
			stages.emplace_back(vertex_stage);

			VkPipelineShaderStageCreateInfo fragment_stage = CreatePipelineShaderStageInfo();
			fragment_stage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragment_stage.module = _fragment_shader;
			fragment_stage.pName = _fragment_shader.Entrypoint().c_str();
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
			info.width = Swapchain().Extent().width;
			info.height = Swapchain().Extent().height;
			info.layers = 1;
			return info;
		}

		VkCommandPoolCreateInfo CreateCommandPoolInfo()
		{
			VkCommandPoolCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			info.queueFamilyIndex = Device().QueueFamilyIndices().graphics.value();
			info.flags = 0; // Optional
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
			info.renderArea.extent = Swapchain().Extent();
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

		VkBufferCreateInfo CreateBufferInfo()
		{
			VkBufferCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			return info;
		}

		VkMemoryAllocateInfo CreateMemoryAllocateInfo()
		{
			VkMemoryAllocateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			return info;
		}

		ui32 ChooseMemoryTypeIndex(ui32 type_filter, VkMemoryPropertyFlags property_flags)
		{
			VkPhysicalDeviceMemoryProperties memory_properties;
			vkGetPhysicalDeviceMemoryProperties(Device().PhysicalDevice(), &memory_properties);

			bl found = false;
			ui32 memory_type_index = 0;

			for (ui32 i = 0; i < memory_properties.memoryTypeCount; i++)
			{
				if ((type_filter & (1 << i)) &&
					(memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
				{
					found = true;
					memory_type_index = i;
				}
			}

			return found ? memory_type_index : -1;
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

			if (vkCreateRenderPass(Device(), &render_pass_info, nullptr, &render_pass) != VK_SUCCESS)
			{
				render_pass = nullptr;
			}

			return render_pass;
		}

		VkPipelineLayout CreatePipelineLayout()
		{
			VkPipelineLayout pipeline_layout = nullptr;

			VkPipelineLayoutCreateInfo pipeline_layout_info = CreatePipelineLayoutInfo();
			if (vkCreatePipelineLayout(Device(), &pipeline_layout_info, nullptr, &pipeline_layout) != VK_SUCCESS)
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

				if (vkCreateGraphicsPipelines(Device(), nullptr, 1, &pipeline_info, nullptr, &pipeline) != VK_SUCCESS)
				{
					pipeline = nullptr;
				}
			}

			return pipeline;
		}

		container::vector<VkFramebuffer> CreateFramebuffers()
		{
			container::vector<VkFramebuffer> framebuffers(Swapchain().ImageViews().size());

			for (siz i = 0; i < Swapchain().ImageViews().size(); i++)
			{
				container::vector<VkImageView> image_views = {Swapchain().ImageViews()[i]};

				VkFramebufferCreateInfo framebuffer_info = CreateFramebufferInfo();
				framebuffer_info.renderPass = _render_pass;
				framebuffer_info.attachmentCount = image_views.size();
				framebuffer_info.pAttachments = image_views.data();

				if (vkCreateFramebuffer(Device(), &framebuffer_info, nullptr, &framebuffers[i]) != VK_SUCCESS)
				{
					framebuffers[i] = nullptr;
				}
			}

			return framebuffers;
		}

		VkCommandPool CreateCommandPool()
		{
			VkCommandPool command_pool = nullptr;
			VkCommandPoolCreateInfo command_pool_info = CreateCommandPoolInfo();
			if (vkCreateCommandPool(Device(), &command_pool_info, nullptr, &command_pool) != VK_SUCCESS)
			{
				command_pool = nullptr;
			}
			return command_pool;
		}

		container::vector<VkCommandBuffer> CreateCommandBuffers()
		{
			container::vector<VkCommandBuffer> command_buffers(_framebuffers.size());

			VkCommandBufferAllocateInfo command_buffer_allocate_info = CreateCommandBufferAllocateInfo();
			command_buffer_allocate_info.commandBufferCount = (ui32)command_buffers.size();

			if (vkAllocateCommandBuffers(Device(), &command_buffer_allocate_info, command_buffers.data()) != VK_SUCCESS)
			{
				command_buffers.clear();
			}

			for (siz i = 0; i < command_buffers.size(); i++)
			{
				VkCommandBufferBeginInfo command_buffer_begin_info = CreateCommandBufferBeginInfo();

				if (vkBeginCommandBuffer(command_buffers[i], &command_buffer_begin_info) != VK_SUCCESS)
				{
					command_buffers.clear();
					break;
				}

				container::vector<VkClearValue> clear_values = CreateClearValues();

				VkRenderPassBeginInfo render_pass_begin_info = CreateRenderPassBeginInfo();
				render_pass_begin_info.framebuffer = _framebuffers[i];
				render_pass_begin_info.clearValueCount = clear_values.size();
				render_pass_begin_info.pClearValues = clear_values.data();

				container::vector<VkBuffer> vertex_buffers{_vertex_buffer};
				container::vector<VkDeviceSize> offsets{0};

				vkCmdBeginRenderPass(command_buffers[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
				vkCmdBindPipeline(command_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
				vkCmdBindVertexBuffers(command_buffers[i], 0, (ui32)vertex_buffers.size(), vertex_buffers.data(),
									   offsets.data());
				vkCmdDraw(command_buffers[i], (ui32)_vertices.size(), 1, 0, 0);
				vkCmdEndRenderPass(command_buffers[i]);

				if (vkEndCommandBuffer(command_buffers[i]) != VK_SUCCESS)
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
				if (vkCreateSemaphore(Device(), &semaphore_info, nullptr, &semaphores[i]) != VK_SUCCESS)
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

					if (vkCreateFence(Device(), &fence_info, nullptr, &fences[i]) != VK_SUCCESS)
					{
						fences.clear();
						break;
					}
				}
			}

			return fences;
		}

		VkBuffer CreateVertexBuffer()
		{
			VkBuffer buffer = nullptr;

			VkBufferCreateInfo buffer_info = CreateBufferInfo();
			buffer_info.size = sizeof(_vertices[0]) * _vertices.size();

			if (vkCreateBuffer(Device(), &buffer_info, nullptr, &buffer) != VK_SUCCESS)
			{
				buffer = nullptr;
			}

			return buffer;
		}

		VkDeviceMemory CreateVertexBufferMemory()
		{
			VkDeviceMemory buffer_memory = nullptr;

			VkMemoryRequirements requirements;
			vkGetBufferMemoryRequirements(Device(), _vertex_buffer, &requirements);

			VkMemoryAllocateInfo allocate_info = CreateMemoryAllocateInfo();
			allocate_info.allocationSize = requirements.size;
			allocate_info.memoryTypeIndex = ChooseMemoryTypeIndex(
				requirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			if (vkAllocateMemory(Device(), &allocate_info, nullptr, &buffer_memory) != VK_SUCCESS)
			{
				buffer_memory = nullptr;
			}
			else
			{
				vkBindBufferMemory(Device(), _vertex_buffer, buffer_memory, 0);
			}

			return buffer_memory;
		}

		void RebuildSwapchain()
		{
			SetRebuildSwapchain(false);

			vkDeviceWaitIdle(Device());

			vkFreeCommandBuffers(Device(), _command_pool, (ui32)_command_buffers.size(), _command_buffers.data());

			for (auto framebuffer : _framebuffers)
				vkDestroyFramebuffer(Device(), framebuffer, nullptr);

			vkDestroyPipeline(Device(), _pipeline, nullptr);
			vkDestroyPipelineLayout(Device(), _pipeline_layout, nullptr);
			vkDestroyRenderPass(Device(), _render_pass, nullptr);

			_swapchain.Rebuild();

			_render_pass = CreateRenderPass();
			_pipeline_layout = CreatePipelineLayout();
			_pipeline = CreatePipeline();
			_framebuffers = CreateFramebuffers();
			_command_buffers = CreateCommandBuffers();
		}

	public:
		VulkanPipeline(VulkanDevice& device):
			_swapchain(device),
			_vertices(3),
			_vertex_buffer(CreateVertexBuffer()),
			_vertex_buffer_memory(CreateVertexBufferMemory()),
			_vertex_shader(Device(), fs::append(fs::append("Vulkan", "shaders"), "vertex.glsl"), VulkanShader::Type::VERTEX),
			_fragment_shader(Device(), fs::append(fs::append("Vulkan", "shaders"), "fragment.glsl"),
							 VulkanShader::Type::FRAGMENT),
			_render_pass(CreateRenderPass()),
			_pipeline_layout(CreatePipelineLayout()),
			_pipeline(CreatePipeline()),
			_framebuffers(CreateFramebuffers()), // TODO: can we put this in a VulkanFramebuffer? I don't think so
			_command_pool(CreateCommandPool()), // TODO: can we put this in a VulkanCommand_____?? maybe....?

			_command_buffers(CreateCommandBuffers()),
			_current_frame(0),
			_image_available_semaphores(CreateSemaphores(MAX_FRAMES)),
			_render_finished_semaphores(CreateSemaphores(MAX_FRAMES)),
			_fences(CreateFences(MAX_FRAMES)),
			_image_fences(CreateFences(Swapchain().Images().size(), false)),
			_rebuild_swapchain(false)
		{
			Surface().Window().SetResizeCallback(this, WindowResizeCallback);
			Surface().Window().SetPositionCallback(this, WindowPositionCallback);

			const container::vector<Vertex> vertices = {
				{{0.0f, -0.5f}, {1.0f, 0.0f, 1.0f}}, {{0.5f, 0.5f}, {1.0f, 1.0f, 0.0f}}, {{-0.5f, 0.5f}, {0.0f, 1.0f, 1.0f}}};

			siz data_size = sizeof(_vertices[0]) * _vertices.size();
			void* data;
			vkMapMemory(device, _vertex_buffer_memory, 0, data_size, 0, &data);
			memcpy(data, vertices.data(), data_size);
			vkUnmapMemory(device, _vertex_buffer_memory);
		}

		~VulkanPipeline()
		{
			vkDeviceWaitIdle(Device());

			vkDestroyBuffer(Device(), _vertex_buffer, nullptr);
			vkFreeMemory(Device(), _vertex_buffer_memory, nullptr);

			for (VkSemaphore& semaphore : _render_finished_semaphores)
				vkDestroySemaphore(Device(), semaphore, nullptr);

			for (VkSemaphore& semaphore : _image_available_semaphores)
				vkDestroySemaphore(Device(), semaphore, nullptr);

			for (VkFence& fence : _fences)
				vkDestroyFence(Device(), fence, nullptr);

			vkDestroyCommandPool(Device(), _command_pool, nullptr);

			for (auto framebuffer : _framebuffers)
				vkDestroyFramebuffer(Device(), framebuffer, nullptr);

			vkDestroyPipeline(Device(), _pipeline, nullptr);
			vkDestroyPipelineLayout(Device(), _pipeline_layout, nullptr);
			vkDestroyRenderPass(Device(), _render_pass, nullptr);
		}

		VulkanInstance& Instance() const
		{
			return _swapchain.Instance();
		}

		VulkanSurface& Surface() const
		{
			return _swapchain.Surface();
		}

		VulkanDevice& Device() const
		{
			return _swapchain.Device();
		}

		const VulkanSwapchain& Swapchain() const
		{
			return _swapchain;
		}

		const VulkanShader& VertexShader() const
		{
			return _vertex_shader;
		}

		const VulkanShader& FragmentShader() const
		{
			return _fragment_shader;
		}

		void Draw(time::DurationMilliseconds time_delta)
		{
			i32 width = 0;
			i32 height = 0;
			glfwGetFramebufferSize((GLFWwindow*)Surface().Window().GetNativeWindow(), &width, &height);
			if (width == 0 || height == 0)
			{
				return;
			}

			vkWaitForFences(Device(), 1, &_fences[_current_frame], VK_TRUE, UI64_MAX);

			ui32 image_index;
			VkResult acquire_result = vkAcquireNextImageKHR(Device(), Swapchain(), UI64_MAX,
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
				vkWaitForFences(Device(), 1, &_image_fences[image_index], VK_TRUE, UI64_MAX);
			}
			// Mark the image as now being in use by this frame
			_image_fences[image_index] = _fences[_current_frame];

			container::vector<VkSemaphore> wait_semaphores{_image_available_semaphores[_current_frame]};
			container::vector<VkPipelineStageFlags> wait_stages{VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			container::vector<VkSemaphore> signal_semaphores{_render_finished_semaphores[_current_frame]};
			container::vector<VkSwapchainKHR> swapchains{Swapchain()};

			VkSubmitInfo submit_info{};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submit_info.waitSemaphoreCount = wait_semaphores.size();
			submit_info.pWaitSemaphores = wait_semaphores.data();
			submit_info.pWaitDstStageMask = wait_stages.data();
			submit_info.commandBufferCount = 1; // _command_buffers.size();
			submit_info.pCommandBuffers = &_command_buffers[image_index];
			submit_info.signalSemaphoreCount = signal_semaphores.size();
			submit_info.pSignalSemaphores = signal_semaphores.data();

			vkResetFences(Device(), 1, &_fences[_current_frame]);

			if (vkQueueSubmit(Device().GraphicsDeviceQueue(), 1, &submit_info, _fences[_current_frame]) != VK_SUCCESS)
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

			VkResult present_result = vkQueuePresentKHR(Device().PresentDeviceQueue(), &present_info);

			if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR || _rebuild_swapchain)
			{
				RebuildSwapchain();
			}
			else if (present_result != VK_SUCCESS)
			{
				NP_ASSERT(false, "vkQueuePresentKHR error");
			}

			vkQueueWaitIdle(Device().PresentDeviceQueue());

			_current_frame = (_current_frame + 1) % MAX_FRAMES;
		}

		operator VkPipeline() const
		{
			return _pipeline;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_PIPELINE_HPP */