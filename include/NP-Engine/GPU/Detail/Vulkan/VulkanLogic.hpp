//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_LOGIC_HPP
#define NP_ENGINE_GPU_VULKAN_LOGIC_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Logic.hpp"

namespace np::gpu::__detail
{
	class VulkanLogicOperation : public LogicOperation
	{
	public:
		VulkanLogicOperation(ui32 value): LogicOperation(value) {}

		VkLogicOp GetVkLogicOp() const
		{
			VkLogicOp op = VK_LOGIC_OP_MAX_ENUM;

			//TODO: for the cases where I'm using Equals exclusively, I could just use a switch statement instead

			switch (_value)
			{
			case Clear:
				op = VK_LOGIC_OP_CLEAR;
				break;
			case Xor:
				op = VK_LOGIC_OP_XOR;
				break;
			case Equal:
				op = VK_LOGIC_OP_EQUIVALENT;
				break;
			case Invert:
				op = VK_LOGIC_OP_INVERT;
				break;
			case NoOp:
				op = VK_LOGIC_OP_NO_OP;
				break;
			case And:
				op = VK_LOGIC_OP_AND;
				break;
			case And | Reverse:
				op = VK_LOGIC_OP_AND_REVERSE;
				break;
			case And | Invert:
				op = VK_LOGIC_OP_AND_INVERTED;
				break;
			case And | Not:
				op = VK_LOGIC_OP_NAND;
				break;
			case Or:
				op = VK_LOGIC_OP_OR;
				break;
			case Or | Not:
				op = VK_LOGIC_OP_NOR;
				break;
			case Or | Reverse:
				op = VK_LOGIC_OP_OR_REVERSE;
				break;
			case Or | Invert:
				op = VK_LOGIC_OP_OR_INVERTED;
				break;
			case Copy:
				op = VK_LOGIC_OP_COPY;
				break;
			case Copy | Invert:
				op = VK_LOGIC_OP_COPY_INVERTED;
				break;
			};

			return op;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_LOGIC_HPP */