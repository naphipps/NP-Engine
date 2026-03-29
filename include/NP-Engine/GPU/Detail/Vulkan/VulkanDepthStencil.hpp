//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_DEPTH_STENCIL_HPP
#define NP_ENGINE_GPU_VULKAN_DEPTH_STENCIL_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanCompareOperation.hpp"

namespace np::gpu::__detail
{
	class VulkanDepthStencilUsage : public DepthStencilUsage
	{
	public:
		VulkanDepthStencilUsage(ui32 value): DepthStencilUsage(value) {}

		VkBool32 IsVkDepthTestEnabled() const
		{
			return Contains(DepthTest) ? VK_TRUE
									   : VK_FALSE; //TODO: we should do this more often than using VK_TRUE\VK_FALSE everywhere
		}

		VkBool32 IsVkDepthWriteEnabled() const
		{
			return Contains(DepthWrite) ? VK_TRUE : VK_FALSE;
		}

		VkBool32 IsVkBoundsTestEnabled() const
		{
			return Contains(BoundsTest) ? VK_TRUE : VK_FALSE;
		}

		VkBool32 IsVkStencilTestEnabled() const
		{
			return Contains(StencilTest) ? VK_TRUE : VK_FALSE;
		}
	};

	class VulkanStencilOperation : public StencilOperation
	{
	public:
		VulkanStencilOperation(ui32 value): StencilOperation(value) {}

		VkStencilOp GetVkStencilOp() const
		{
			VkStencilOp op = VK_STENCIL_OP_MAX_ENUM;

			switch (_value)
			{
			case Keep:
				op = VK_STENCIL_OP_KEEP;
				break;
			case Zero:
				op = VK_STENCIL_OP_ZERO;
				break;
			case Replace:
				op = VK_STENCIL_OP_REPLACE;
				break;
			case Increment | Clamp:
				op = VK_STENCIL_OP_INCREMENT_AND_CLAMP;
				break;
			case Decrement | Clamp:
				op = VK_STENCIL_OP_DECREMENT_AND_CLAMP;
				break;
			case Invert:
				op = VK_STENCIL_OP_INVERT;
				break;
			case Increment | Wrap:
				op = VK_STENCIL_OP_INCREMENT_AND_WRAP;
				break;
			case Decrement | Wrap:
				op = VK_STENCIL_OP_DECREMENT_AND_WRAP;
				break;
			};

			return op;
		}
	};

	struct VulkanStencilOperationState
	{
		VulkanStencilOperation stencilFailOperation = VulkanStencilOperation::None;
		VulkanStencilOperation stencilPassOperation = VulkanStencilOperation::None;
		VulkanStencilOperation depthFailOperation = VulkanStencilOperation::None;
		VulkanCompareOperation compareOperation = VulkanCompareOperation::None;

		ui32 compareMask = 0;
		ui32 writeMask = 0;
		ui32 referenceValue = 0;

		VulkanStencilOperationState() = default;

		VulkanStencilOperationState(const StencilOperationState& other):
			stencilFailOperation(other.stencilFailOperation),
			stencilPassOperation(other.stencilPassOperation),
			depthFailOperation(other.depthFailOperation),
			compareOperation(other.compareOperation),
			compareMask(other.compareMask),
			writeMask(other.writeMask),
			referenceValue(other.referenceValue)
		{}

		operator StencilOperationState() const
		{
			return {stencilFailOperation, stencilPassOperation, depthFailOperation, compareOperation, compareMask, writeMask,
					referenceValue};
		}

		VkStencilOpState GetVkStencilOpState() const
		{
			VkStencilOpState state{};
			state.failOp = stencilFailOperation.GetVkStencilOp();
			state.passOp = stencilPassOperation.GetVkStencilOp();
			state.depthFailOp = depthFailOperation.GetVkStencilOp();
			state.compareOp = compareOperation.GetVkCompareOp();
			state.compareMask = compareMask;
			state.writeMask = writeMask;
			state.reference = referenceValue;
			return state;
		}
	};

	struct VulkanDepthBounds : public mat::range_flt
	{
		VulkanDepthBounds() = default;

		VulkanDepthBounds(const DepthBounds& other)
		{
			min = other.min;
			max = other.max;
		}

		operator DepthBounds() const
		{
			return {min, max};
		}
	};

	struct VulkanDepthStencil
	{
		VulkanDepthStencilUsage usage = VulkanDepthStencilUsage::None;
		VulkanCompareOperation depthCompareOperation = VulkanCompareOperation::None;
		VulkanStencilOperationState frontState{};
		VulkanStencilOperationState backState{};
		VulkanDepthBounds depthBounds{};

		VulkanDepthStencil() = default;

		VulkanDepthStencil(const DepthStencil& other):
			usage(other.usage),
			depthCompareOperation(other.depthCompareOperation),
			frontState(other.frontState),
			backState(other.backState),
			depthBounds(other.depthBounds)
		{}

		operator DepthStencil() const
		{
			return {usage, depthCompareOperation, frontState, backState, depthBounds};
		}
	};

	class VulkanStencilFace : public StencilFace
	{
	public:
		VulkanStencilFace(ui32 value): StencilFace(value) {}

		VkStencilFaceFlags GetVkStencilFaceFlags() const
		{
			VkStencilFaceFlags flags = 0;

			if (Contains(Front))
				flags |= VK_STENCIL_FACE_FRONT_BIT;
			if (Contains(Back))
				flags |= VK_STENCIL_FACE_BACK_BIT;
			if (Contains(Front | Back))
				flags |= VK_STENCIL_FACE_FRONT_AND_BACK;

			return flags;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_DEPTH_STENCIL_HPP */