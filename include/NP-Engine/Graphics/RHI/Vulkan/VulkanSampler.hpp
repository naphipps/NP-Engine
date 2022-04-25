//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/4/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SAMPLER_HPP
#define NP_ENGINE_VULKAN_SAMPLER_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "VulkanDevice.hpp"

namespace np::graphics::rhi
{
	class VulkanSampler
	{
	private:
		VulkanDevice& _device;
		VkSampler _sampler;

		VkSampler CreateSampler(VkSamplerCreateInfo& info)
		{
			VkPhysicalDeviceProperties device_properties;
			VkPhysicalDeviceFeatures device_features;
			vkGetPhysicalDeviceProperties(GetDevice().GetPhysicalDevice(), &device_properties);
			vkGetPhysicalDeviceFeatures(GetDevice().GetPhysicalDevice(), &device_features);

			info.anisotropyEnable = device_features.samplerAnisotropy; // TODO: revisit
			info.maxAnisotropy = info.anisotropyEnable == VK_FALSE ? 1.0f : // TODO: revisit
				device_properties.limits.maxSamplerAnisotropy;

			VkSampler sampler = nullptr;

			if (vkCreateSampler(GetDevice(), &info, nullptr, &sampler) != VK_SUCCESS)
			{
				sampler = nullptr;
			}

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
			info.compareEnable = VK_FALSE; // TODO: revisit
			info.compareOp = VK_COMPARE_OP_ALWAYS; // TODO: revisit
			info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // TODO: revisit
			info.mipLodBias = 0.0f; // TODO: revisit
			info.minLod = 0.0f; // TODO: revisit
			info.maxLod = 0.0f; // TODO: revisit
			return info;
		}

		VulkanSampler(VulkanDevice& device, VkSamplerCreateInfo sampler_create_info):
			_device(device),
			_sampler(CreateSampler(sampler_create_info))
		{}

		~VulkanSampler()
		{
			if (_sampler)
				vkDestroySampler(GetDevice(), _sampler, nullptr);
		}

		operator VkSampler() const
		{
			return _sampler;
		}

		VulkanDevice& GetDevice() const
		{
			return _device;
		}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SAMPLER_HPP */