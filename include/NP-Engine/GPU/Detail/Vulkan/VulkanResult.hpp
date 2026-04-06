//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/29/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_RESULT_HPP
#define NP_ENGINE_GPU_VULKAN_RESULT_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Result.hpp"

namespace np::gpu::__detail
{
	class VulkanResult : public Result
	{
	protected:
		static Result ToResult(VkResult vk_result)
		{
			Result result = Result::None;

			switch (vk_result)
			{
			case VK_ERROR_OUT_OF_DATE_KHR:
				result = Result::OutOfDate;
				break;

			case VK_SUBOPTIMAL_KHR:
				result = Result::Success | Result::Suboptimal;
				break;

			case VK_SUCCESS:
				result = Result::Success;
				break;

			//TODO: VK_NOT_READY?
			default:
				result = Result::Error;
				break;
			}

			return result;
		}

	public:
		VulkanResult(ui32 value) : Result(value) {}

		VulkanResult(VkResult vk_result) : Result(ToResult(vk_result)) {}

		VkResult GetVkFormat() const
		{
			VkResult result = VK_RESULT_MAX_ENUM;

			if (Contains(Success))
				result = VK_SUCCESS;
			if (Contains(Suboptimal))
				result = VK_SUBOPTIMAL_KHR;
			if (Contains(OutOfDate))
				result = VK_ERROR_OUT_OF_DATE_KHR;

			return result;
		}
	};
} //namespace np::gpu

#endif /* NP_ENGINE_GPU_VULKAN_RESULT_HPP */