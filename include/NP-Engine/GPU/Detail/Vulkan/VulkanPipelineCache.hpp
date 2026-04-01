//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/15/25
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_PIPELINE_CACHE_HPP
#define NP_ENGINE_GPU_VULKAN_PIPELINE_CACHE_HPP

#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gpu::__detail
{
	class VulkanPipelineCache : public PipelineCache
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkPipelineCache _cache;

		static VkPipelineCacheCreateInfo CreateVkInfo()
		{
			VkPipelineCacheCreateInfo info = {};
			info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			return info;
		}

		static VkPipelineCache CreateVkPipelineCache(mem::sptr<VulkanLogicalDevice> device, const con::vector<ui8>& bytes)
		{
			VkPipelineCacheCreateInfo info = CreateVkInfo();
			info.initialDataSize = bytes.size();
			info.pInitialData = info.initialDataSize > 0 ? bytes.data() : nullptr;

			mem::sptr<VulkanInstance> instance = device->GetPhysicalDevice().GetDetailInstance();
			VkPipelineCache cache = nullptr;
			VkResult result = vkCreatePipelineCache(*device, &info, instance->GetVulkanAllocationCallbacks(), &cache);
			return result == VK_SUCCESS ? cache : nullptr;
		}

	public:
		VulkanPipelineCache(mem::sptr<VulkanLogicalDevice> device, con::vector<ui8> bytes):
			PipelineCache(),
			_device(device),
			_cache(CreateVkPipelineCache(_device, bytes))
		{}

		virtual ~VulkanPipelineCache()
		{
			vkDeviceWaitIdle(*_device);

			if (_cache)
			{
				mem::sptr<VulkanInstance> instance = _device->GetPhysicalDevice().GetDetailInstance();
				vkDestroyPipelineCache(*_device, _cache, instance->GetVulkanAllocationCallbacks());
				_cache = nullptr;
			}
		}

		operator VkPipelineCache() const
		{
			return _cache;
		}

		mem::sptr<VulkanLogicalDevice> GetLogicalDevice() const
		{
			return _device;
		}

		mem::sptr<srvc::Services> GetServices() const
		{
			return _device->GetServices();
		}

		virtual bl Absorb(mem::sptr<PipelineCache> cache) override
		{
			return false; //TODO: absorb given cache!
		}

		virtual con::vector<ui8> GetBytes() const override
		{
			siz size = 0;
			vkGetPipelineCacheData(*_device, _cache, &size, nullptr);
			con::vector<ui8> bytes(size);
			vkGetPipelineCacheData(*_device, _cache, &size, bytes.data());
			return bytes;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_PIPELINE_CACHE_HPP */