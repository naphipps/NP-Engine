//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_STAGE_HPP
#define NP_ENGINE_GPU_VULKAN_STAGE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

namespace np::gpu::__detail
{
	class VulkanStage : public Stage
	{
	public:
		VulkanStage(ui32 value): Stage(value) {}

		VkPipelineStageFlags GetVkPipelineStageFlags() const
		{
			VkPipelineStageFlags flags = VK_PIPELINE_STAGE_NONE;

			if (Contains(Top))
				flags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			if (Contains(VertexInput))
				flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
			if (Contains(VertexOutput))
				flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
			if (Contains(TesselationControl))
				flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
			if (Contains(TesselationEvaluation))
				flags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
			if (Contains(Geometry))
				flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
			if (Contains(FragmentInput))
				flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			if (Contains(FragmentOutput))
				flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
			if (Contains(PresentComplete))
				flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
			if (Contains(Compute))
				flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
			if (Contains(Transfer))
				flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;
			if (Contains(Bottom))
				flags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
			if (_value == All)
				flags |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT | VK_PIPELINE_STAGE_FLAG_BITS_MAX_ENUM;

			//TODO: figure out VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT

			return flags;
		}

		VkShaderStageFlags GetVkShaderStageFlags() const
		{
			VkShaderStageFlags flags = 0;

			if (ContainsAny(Vertex))
				flags |= VK_SHADER_STAGE_VERTEX_BIT;
			if (Contains(TesselationControl))
				flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			if (Contains(TesselationEvaluation))
				flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			if (Contains(Geometry))
				flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
			if (ContainsAny(Fragment))
				flags |= VK_SHADER_STAGE_FRAGMENT_BIT;
			if (Contains(Compute))
				flags |= VK_SHADER_STAGE_COMPUTE_BIT;
			if (_value == All)
				flags = VK_SHADER_STAGE_ALL;

			return flags;
		}

		VkShaderStageFlagBits GetVkShaderStageFlagBits() const
		{
			VkShaderStageFlagBits bit{};

			switch (_value)
			{
			case Vertex:
				bit = VK_SHADER_STAGE_VERTEX_BIT;
				break;

			case TesselationControl:
				bit = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
				break;

			case TesselationEvaluation:
				bit = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
				break;

			case Geometry:
				bit = VK_SHADER_STAGE_GEOMETRY_BIT;
				break;

			case Fragment:
				bit = VK_SHADER_STAGE_FRAGMENT_BIT;
				break;

			case Compute:
				bit = VK_SHADER_STAGE_COMPUTE_BIT;
				break;

			case All:
				bit = VK_SHADER_STAGE_ALL;
				break;
			}

			return bit;
		}

		str GetCompileName() const
		{
			str name = "UNKNOWN";

			switch (_value)
			{
			case Vertex:
				name = "vertex";
				break;

			case TesselationControl:
				name = "tesscontrol";
				break;

			case TesselationEvaluation:
				name = "tesseval";
				break;

			case Geometry:
				name = "geometry";
				break;

			case Fragment:
				name = "fragment";
				break;

			case Compute:
				name = "compute";
				break;
			}

			return name;
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_STAGE_HPP */