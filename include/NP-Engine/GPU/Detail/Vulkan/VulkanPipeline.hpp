//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/2/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VUKLAN_PIPELINE_HPP
#define NP_ENGINE_GPU_VUKLAN_PIPELINE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanDevice.hpp"
#include "VulkanPipelineCache.hpp"
#include "VulkanResource.hpp"
#include "VulkanPushData.hpp"

namespace np::gpu::__detail
{
	class VulkanPipelineUsage : public PipelineUsage
	{
	public:
		VulkanPipelineUsage(ui32 value): PipelineUsage(value) {}

		VkPipelineBindPoint GetVkPipelineBindPoint() const
		{
			VkPipelineBindPoint point = VK_PIPELINE_BIND_POINT_MAX_ENUM;

			if (Contains(Graphics))
				point = VK_PIPELINE_BIND_POINT_GRAPHICS;
			else if (Contains(Compute))
				point = VK_PIPELINE_BIND_POINT_COMPUTE;

			return point;
		}

		VkPipelineCreateFlags GetVkPipelineCreateFlags() const
		{
			VkPipelineCreateFlags flags = 0;

			if (Contains(QuickStartup))
				flags |= VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;

			//TODO: investigate VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT and VK_PIPELINE_CREATE_DERIVATIVE_BIT

			return flags;
		}
	};

	class VulkanPipelineResourceLayout : public PipelineResourceLayout
	{
	private:
		mem::sptr<VulkanDevice> _device; //TODO: if VulkanLogicalDevice is used, mention it (aka name it _logical_device)
		con::vector<mem::sptr<VulkanResourceLayout>> _resource_layouts;
		PushData _push_data;
		VkPipelineLayout _pipeline_layout;

		static VkPipelineLayoutCreateInfo CreateVkInfo()
		{
			VkPipelineLayoutCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			return info;
		}

		static VkPipelineLayout CreateVkPipelineLayout(mem::sptr<VulkanDevice> device,
													   const con::vector<mem::sptr<VulkanResourceLayout>>& resource_layouts,
													   const PushData& push_data)
		{
			con::vector<VkDescriptorSetLayout> layouts(resource_layouts.size());
			for (siz i = 0; i < layouts.size(); i++)
				layouts[i] = *resource_layouts[i];

			ui32 offset = 0;
			const con::vector<ui8> push_data_bytes = push_data.GetAllEntryBytes();
			con::vector<VkPushConstantRange> ranges(push_data.entries.size());
			for (siz i = 0; i < ranges.size(); i++)
			{
				const PushDataEntry& entry = push_data.entries[i];
				VkPushConstantRange& range = ranges[i];
				range = {VulkanStage{entry.stage}, offset, (ui32)entry.bytes.size()};
				offset += range.size;
			}

			VkPipelineLayoutCreateInfo info = CreateVkInfo();
			info.setLayoutCount = (ui32)layouts.size();
			info.pSetLayouts = layouts.empty() ? nullptr : layouts.data();
			info.pushConstantRangeCount = (ui32)ranges.size();
			info.pPushConstantRanges = ranges.empty() ? nullptr : ranges.data();

			mem::sptr<VulkanInstance> instance = device->GetDetailInstance();
			VkPipelineLayout layout = nullptr;
			VkResult result = vkCreatePipelineLayout(*device->GetLogicalDevice(), &info, instance->GetVulkanAllocationCallbacks(), &layout);
			return result == VK_SUCCESS ? layout : nullptr;
		}

		static bl IsPushDataEntryCompatible(const PushDataEntry& a, const PushDataEntry& b)
		{
			return a.stage == b.stage && a.bytes.size() == b.bytes.size();
		}

	public:
		//TODO: make sure all the things are checking our physical device's limits
		VulkanPipelineResourceLayout(mem::sptr<Device> device,
			const con::vector<mem::sptr<ResourceLayout>>& resource_layouts, PushData push_data): 
			PipelineResourceLayout(),
			_device(device),
			_resource_layouts(resource_layouts.begin(), resource_layouts.end()),
			_push_data(push_data),
			_pipeline_layout(CreateVkPipelineLayout(_device, _resource_layouts, _push_data))
		{}

		virtual ~VulkanPipelineResourceLayout()
		{
			if (_pipeline_layout)
			{
				mem::sptr<VulkanInstance> instance = _device->GetDetailInstance();
				vkDestroyPipelineLayout(*_device->GetLogicalDevice(), _pipeline_layout, instance->GetVulkanAllocationCallbacks());
				_pipeline_layout = nullptr;
			}
		}

		operator VkPipelineLayout() const
		{
			return _pipeline_layout;
		}

		virtual DetailType GetDetailType() const override
		{
			return _device->GetDetailType();
		}

		virtual mem::sptr<srvc::Services> GetServices() const override
		{
			return _device->GetServices();
		}

		virtual mem::sptr<Device> GetDevice() const override
		{
			return _device;
		}

		virtual con::vector<mem::sptr<ResourceLayout>> GetResourceLayouts() const override
		{
			return {_resource_layouts.begin(), _resource_layouts.end()};
		}

		virtual PushData GetPushData() const override
		{
			return _push_data;
		}

		virtual bl SetPushDataEntry(siz index, const PushDataEntry& entry) override
		{
			bl set = false;
			if (index < _push_data.entries.size())
			{
				PushDataEntry& e = _push_data.entries[index];
				if (IsPushDataEntryCompatible(e, entry))
				{
					e = entry;
					set = true;
				}
			}
			return set;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VUKLAN_PIPELINE_HPP */