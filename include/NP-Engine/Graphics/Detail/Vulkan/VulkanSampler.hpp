//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SAMPLER_HPP
#define NP_ENGINE_VULKAN_SAMPLER_HPP

#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanLogicalDevice.hpp"

namespace np::gfx::__detail
{
	class VulkanSampler
	{
	private:
		mem::sptr<VulkanLogicalDevice> _device;
		VkSampler _sampler;

		static VkSampler CreateSampler(mem::sptr<VulkanLogicalDevice> device, VkSamplerCreateInfo& info)
		{
			VkPhysicalDeviceProperties device_properties;
			VkPhysicalDeviceFeatures device_features;
			vkGetPhysicalDeviceProperties(device->GetPhysicalDevice(), &device_properties);
			vkGetPhysicalDeviceFeatures(device->GetPhysicalDevice(), &device_features);

			info.anisotropyEnable = device_features.samplerAnisotropy;
			info.maxAnisotropy = info.anisotropyEnable == VK_FALSE ? 1.0f : device_properties.limits.maxSamplerAnisotropy;

			VkSampler sampler = nullptr;
			if (vkCreateSampler(*device, &info, nullptr, &sampler) != VK_SUCCESS)
				sampler = nullptr;

			return sampler;
		}

	public:
		static VkSamplerCreateInfo CreateInfo()
		{
			VkSamplerCreateInfo info{};
			info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			info.magFilter = VK_FILTER_LINEAR;
			info.minFilter = VK_FILTER_LINEAR;
			info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			info.anisotropyEnable = VK_TRUE;
			info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			info.unnormalizedCoordinates = VK_FALSE;
			info.compareEnable = VK_FALSE;
			info.compareOp = VK_COMPARE_OP_ALWAYS;
			info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			info.mipLodBias = 0.0f;
			info.minLod = 0.0f;
			info.maxLod = 0.0f;
			return info;
		}

		VulkanSampler(mem::sptr<VulkanLogicalDevice> device, VkSamplerCreateInfo sampler_create_info):
			_device(device),
			_sampler(CreateSampler(_device, sampler_create_info))
		{}

		~VulkanSampler()
		{
			if (_sampler)
			{
				vkDestroySampler(*_device, _sampler, nullptr);
				_sampler = nullptr;
			}
		}

		operator VkSampler() const
		{
			return _sampler;
		}

		mem::sptr<VulkanLogicalDevice> GetDevice() const
		{
			return _device;
		}
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_SAMPLER_HPP */