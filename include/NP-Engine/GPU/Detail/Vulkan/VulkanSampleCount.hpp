//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/6/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_SAMPLE_COUNT_HPP
#define NP_ENGINE_GPU_VULKAN_SAMPLE_COUNT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

namespace np::gpu::__detail
{
	class VulkanSampleCount
	{
	private:
		ui32 _value;

	public:
		constexpr static VkSampleCountFlags AllVkSampleCountFlags = VK_SAMPLE_COUNT_1_BIT | VK_SAMPLE_COUNT_2_BIT |
			VK_SAMPLE_COUNT_4_BIT | VK_SAMPLE_COUNT_8_BIT | VK_SAMPLE_COUNT_16_BIT | VK_SAMPLE_COUNT_32_BIT |
			VK_SAMPLE_COUNT_64_BIT;

		constexpr static con::array<VkSampleCountFlagBits, 7> AllVkSampleCountFlagBitsArray{
			VK_SAMPLE_COUNT_1_BIT,	VK_SAMPLE_COUNT_2_BIT,	VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_8_BIT,
			VK_SAMPLE_COUNT_16_BIT, VK_SAMPLE_COUNT_32_BIT, VK_SAMPLE_COUNT_64_BIT};

		VulkanSampleCount(ui32 value = 0): _value(value) {}

		operator ui32() const
		{
			return _value;
		}

		VkSampleCountFlagBits GetVkSampleCountFlagBits() const
		{
			VkSampleCountFlagBits bit = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;

			if (_value > 32)
				bit = VK_SAMPLE_COUNT_64_BIT;
			else if (_value > 16)
				bit = VK_SAMPLE_COUNT_32_BIT;
			else if (_value > 8)
				bit = VK_SAMPLE_COUNT_16_BIT;
			else if (_value > 4)
				bit = VK_SAMPLE_COUNT_8_BIT;
			else if (_value > 2)
				bit = VK_SAMPLE_COUNT_4_BIT;
			else if (_value > 1)
				bit = VK_SAMPLE_COUNT_2_BIT;
			else if (_value > 0)
				bit = VK_SAMPLE_COUNT_1_BIT;

			return bit;
		}

		VkSampleCountFlags GetVkSampleCountFlags() const
		{
			return _value & AllVkSampleCountFlags;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_SAMPLE_COUNT_HPP */