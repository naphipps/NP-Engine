//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_BLEND_HPP
#define NP_ENGINE_GPU_VULKAN_BLEND_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanLogic.hpp"
#include "VulkanColor.hpp"

namespace np::gpu::__detail
{
	class VulkanBlendScalar : public BlendScalar
	{
	public:
		//constexpr static ui32 Src1 = BIT(0); //TODO: investigate

		VulkanBlendScalar(ui32 value) : BlendScalar(value) {}

		VkBlendFactor GetVkBlendFactor() const
		{
			VkBlendFactor factor = VK_BLEND_FACTOR_MAX_ENUM;

			switch (_value)
			{
			case Zeroed:
				factor = VK_BLEND_FACTOR_ZERO;
				break;
			case Oned:
				factor = VK_BLEND_FACTOR_ONE;
				break;

			case Src:
				factor = VK_BLEND_FACTOR_SRC_COLOR;
				break;
			case Src | OneMinus:
				factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
				break;
			case Src | Alpha:
				factor = VK_BLEND_FACTOR_SRC_ALPHA;
				break;
			case Src | Alpha | OneMinus:
				factor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
				break;

			case Dst:
				factor = VK_BLEND_FACTOR_DST_COLOR;
				break;
			case Dst | OneMinus:
				factor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
				break;
			case Dst | Alpha:
				factor = VK_BLEND_FACTOR_DST_ALPHA;
				break;
			case Dst | Alpha | OneMinus:
				factor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
				break;

			case Color | Constant:
				factor = VK_BLEND_FACTOR_CONSTANT_COLOR;
				break;
			case Color | Constant | OneMinus:
				factor = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
				break;
			case Color | Alpha:
				factor = VK_BLEND_FACTOR_CONSTANT_ALPHA;
				break;
			case Color | Alpha | OneMinus:
				factor = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
				break;

			case Alpha | Saturate:
				factor = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
				break;
			};

			return factor;
		}
	};

	class VulkanBlendOperation : public BlendOperation
	{
	public:
		VulkanBlendOperation(ui32 value) : BlendOperation(value) {}

		VkBlendOp GetVkBlendOp() const
		{
			VkBlendOp op = VK_BLEND_OP_MAX_ENUM;

			switch (_value)
			{
			case Add:
				op = VK_BLEND_OP_ADD;
				break;
			case Subtract:
				op = VK_BLEND_OP_SUBTRACT;
				break;
			case Subtract | Reverse:
				op = VK_BLEND_OP_REVERSE_SUBTRACT;
				break;
			case Min:
				op = VK_BLEND_OP_MIN;
				break;
			case Max:
				op = VK_BLEND_OP_MAX;
				break;
			};

			return op;
		}
	};

	struct VulkanBlending
	{
		VulkanBlendScalar srcScalar = VulkanBlendScalar::None;
		VulkanBlendScalar dstScalar = VulkanBlendScalar::None;
		VulkanBlendOperation op = VulkanBlendOperation::None;

		VulkanBlending() = default;

		VulkanBlending(const Blending& other) :
			srcScalar(other.srcScalar),
			dstScalar(other.dstScalar),
			op(other.op)
		{}

		operator Blending() const
		{
			return { srcScalar, dstScalar, op };
		}
	};

	struct VulkanResourceBlending
	{
		bl enable = false; //TODO: surely we can get rid of this?
		VulkanBlending color{};
		VulkanBlending alpha{};
		VulkanColorChannel colorChannel = VulkanColorChannel::None;

		bl IsEnabled() const
		{
			return color.srcScalar != VulkanBlendScalar::None || color.dstScalar != VulkanBlendScalar::None || color.op != VulkanBlendOperation::None
				|| alpha.srcScalar != VulkanBlendScalar::None || alpha.dstScalar != VulkanBlendScalar::None || alpha.op != VulkanBlendOperation::None;
		}

		VulkanResourceBlending(const ResourceBlending& other) :
			enable(other.enable),
			color(other.color),
			alpha(other.alpha),
			colorChannel(other.colorChannel)
		{}

		operator ResourceBlending() const
		{
			return { enable, color, alpha, colorChannel };
		}

		VkPipelineColorBlendAttachmentState GetVkPipelineColorBlendAttachmentState() const
		{
			VkPipelineColorBlendAttachmentState state{};
			state.blendEnable = enable ? VK_TRUE : VK_FALSE;
			state.srcColorBlendFactor = color.srcScalar.GetVkBlendFactor();
			state.dstColorBlendFactor = color.dstScalar.GetVkBlendFactor();
			state.colorBlendOp = color.op.GetVkBlendOp();
			state.srcAlphaBlendFactor = alpha.srcScalar.GetVkBlendFactor();
			state.dstAlphaBlendFactor = alpha.dstScalar.GetVkBlendFactor();
			state.alphaBlendOp = alpha.op.GetVkBlendOp();
			state.colorWriteMask = colorChannel.GetVkColorComponentFlags();
			return state;
		}
	};

	struct VulkanBlendConstants : public BlendConstants
	{
		//TODO: whatever these need to be

		VulkanBlendConstants() = default;

		VulkanBlendConstants(const BlendConstants& other) : BlendConstants{ other } {}

		//operator BlendConstants() const {} //TODO: don't forget
	};

	struct VulkanBlend
	{
		VulkanLogicOperation logicOperation = VulkanLogicOperation::None;
		con::vector<VulkanResourceBlending> resourceBlendings{};
		VulkanBlendConstants constants{};

		VulkanBlend(const Blend& other) :
			logicOperation(other.logicOperation),
			resourceBlendings{other.resourceBlendings.begin(), other.resourceBlendings.end()},
			constants(other.constants)
		{}

		operator Blend() const
		{
			return { logicOperation, {resourceBlendings.begin(), resourceBlendings.end()}, constants };
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_VULKAN_BLEND_HPP */