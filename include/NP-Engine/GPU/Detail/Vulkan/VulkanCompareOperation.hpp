//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/8/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_COMPARE_OPERATION_HPP
#define NP_ENGINE_GPU_VULKAN_COMPARE_OPERATION_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

namespace np::gpu::__detail
{
	class VulkanCompareOperation : public CompareOperation
	{
	public:
		VulkanCompareOperation(ui32 value): CompareOperation(value) {}

		VkCompareOp GetVkCompareOp() const
		{
			VkCompareOp op = VK_COMPARE_OP_MAX_ENUM;

			switch (_value)
			{
			case Never:
				op = VK_COMPARE_OP_NEVER;
				break;
			case Less:
			case Less | NotEqual:
				op = VK_COMPARE_OP_LESS;
				break;
			case Equal:
				op = VK_COMPARE_OP_EQUAL;
				break;
			case Less | Equal:
				op = VK_COMPARE_OP_LESS_OR_EQUAL;
				break;
			case Greater:
			case Greater | NotEqual:
				op = VK_COMPARE_OP_GREATER;
				break;
			case NotEqual:
				op = VK_COMPARE_OP_NOT_EQUAL;
				break;
			case Greater | Equal:
				op = VK_COMPARE_OP_GREATER_OR_EQUAL;
				break;
			case Always:
				op = VK_COMPARE_OP_ALWAYS;
				break;
			};

			return op;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_COMPARE_OPERATION_HPP */