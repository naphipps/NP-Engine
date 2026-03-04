//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_MULTISAMPLE_HPP
#define NP_ENGINE_GPU_VULKAN_MULTISAMPLE_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanSampleCount.hpp"

namespace np::gpu::__detail
{
	class VulkanMultisampleUsage : public MultisampleUsage
	{
	public:
		VulkanMultisampleUsage(ui32 value): MultisampleUsage(value) {}

		//TODO: get Vk things

		VulkanSampleCount GetVulkanSampleCount() const
		{
			return GetSampleCount();
		}

		VkBool32 IsVkSampleShading() const
		{
			return Contains(Shading) ? VK_TRUE : VK_FALSE;
		}

		VkBool32 IsVkAlphaToCoverage() const
		{
			return Contains(AlphaToCoverage) ? VK_TRUE : VK_FALSE;
		}

		VkBool32 IsVkAlphaToOne() const
		{
			return Contains(AlphaToOne) ? VK_TRUE : VK_FALSE;
		}
	};

	struct VulkanMultisample
	{
		VulkanMultisampleUsage usage = VulkanMultisampleUsage::None;
		flt minSampleShadingPercentage = 0;
		con::vector<ui32> sampleMasks{};

		VulkanMultisample(const Multisample& other = {}):
			usage(other.usage),
			minSampleShadingPercentage(other.minSampleShadingPercentage)
		{
			sampleMasks.resize(other.sampleMasks.size());
			for (siz i = 0; i < sampleMasks.size(); i++)
				sampleMasks[i] = (ui32)other.sampleMasks[i];
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_MULTISAMPLE_HPP */