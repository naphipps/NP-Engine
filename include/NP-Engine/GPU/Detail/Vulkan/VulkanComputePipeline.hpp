//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/1/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VUKLAN_COMPUTE_PIPELINE_HPP
#define NP_ENGINE_GPU_VUKLAN_COMPUTE_PIPELINE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanPipeline.hpp"
#include "VulkanShader.hpp"

namespace np::gpu::__detail
{
	class VulkanComputePipeline : public ComputePipeline
	{
	protected:
		mem::sptr<VulkanShader> _shader;
		mem::sptr<VulkanPipelineResourceLayout> _layout;
		VkPipeline _pipeline;

		static VkComputePipelineCreateInfo CreateVkInfo(mem::sptr<VulkanShader> shader, mem::sptr<VulkanPipelineResourceLayout> layout, VulkanPipelineUsage usage)
		{
			VkComputePipelineCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			info.flags = usage.GetVkPipelineCreateFlags();
			info.stage = shader->GetVkPipelineShaderStageCreateInfo();
			info.layout = layout ? *layout : (VkPipelineLayout)nullptr;
			return info;
		}

		static VkPipeline CreateVkPipeline(mem::sptr<VulkanShader> shader, mem::sptr<VulkanPipelineResourceLayout> layout, VulkanPipelineUsage usage, mem::sptr<VulkanPipelineCache> cache)
		{
			VkComputePipelineCreateInfo info = CreateVkInfo(shader, layout, usage);
			mem::sptr<VulkanDevice> device = shader->GetDevice();
			VkPipeline pipeline = nullptr;
			VkResult result = vkCreateComputePipelines(*device->GetLogicalDevice(), nullptr, 1, &info, nullptr, &pipeline);
			return result == VK_SUCCESS ? pipeline : nullptr;
		}

	public:
		VulkanComputePipeline(mem::sptr<Shader> shader, mem::sptr<PipelineResourceLayout> layout, PipelineUsage usage, mem::sptr<PipelineCache> cache):
			_shader(shader),
			_layout(layout),
			_pipeline(CreateVkPipeline(_shader, _layout, (ui32)usage, cache))
		{}

		virtual ~VulkanComputePipeline()
		{
			if (_pipeline)
			{
				mem::sptr<VulkanDevice> device = _shader->GetDevice();
				vkDestroyPipeline(*device->GetLogicalDevice(), _pipeline, nullptr);
				_pipeline = nullptr;
			}
		}

		virtual operator VkPipeline() const
		{
			return _pipeline;
		}

		virtual DetailType GetDetailType() const override
		{
			return DetailType::Vulkan;
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _shader->GetServices();
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _shader->GetDevice();
		}

		virtual mem::sptr<Shader> GetShader() const override
		{
			return _shader;
		}

		virtual mem::sptr<PipelineResourceLayout> GetPipelineResourceLayout() const override
		{
			return _layout;
		}
	};
}

#endif /* NP_ENGINE_GPU_VUKLAN_COMPUTE_PIPELINE_HPP */