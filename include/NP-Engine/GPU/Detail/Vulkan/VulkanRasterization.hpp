//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_RASTERIZATION_HPP
#define NP_ENGINE_GPU_VULKAN_RASTERIZATION_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanDepthStencil.hpp"

namespace np::gpu::__detail
{
	class VulkanRasterizationUsage : public RasterizationUsage
	{
	public:
		VulkanRasterizationUsage(ui32 value) : RasterizationUsage(value) {}

		VkPolygonMode GetVkPolygonMode() const
		{
			VkPolygonMode mode = VK_POLYGON_MODE_FILL;

			if (Contains(PolygonFill))
				mode = VK_POLYGON_MODE_FILL;
			else if (Contains(PolygonLine))
				mode = VK_POLYGON_MODE_LINE;
			else if (Contains(PolygonPoint))
				mode = VK_POLYGON_MODE_POINT;

			return mode;
		}

		VkCullModeFlags GetVkCullModeFlags() const
		{
			VkCullModeFlags flags = VK_CULL_MODE_NONE;

			if (Contains(CullBack))
				flags |= VK_CULL_MODE_BACK_BIT;
			else if (Contains(CullFront))
				flags |= VK_CULL_MODE_FRONT_BIT;

			if (Contains(CullFront | CullBack))
				flags |= VK_CULL_MODE_FRONT_AND_BACK;

			return flags;
		}

		VkFrontFace GetVkFrontFace() const
		{
			return Contains(FrontFaceClockwise) ? VK_FRONT_FACE_CLOCKWISE : VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}
	};

	struct VulkanDepthBias
	{
		flt constantScalar = 0;
		flt clamp = 0;
		flt slopeScalar = 0;

		VulkanDepthBias(const DepthBias& other = {}) :
			constantScalar(other.constantScalar),
			clamp(other.clamp),
			slopeScalar(other.slopeScalar)
		{}

		operator DepthBias() const
		{
			return { constantScalar, clamp, slopeScalar };
		}

		VkBool32 IsVkEnabled() const
		{
			return constantScalar != 0 || clamp != 0 || slopeScalar != 0 ? VK_TRUE : VK_FALSE;
		}
	};

	struct VulkanRasterization
	{
		VulkanRasterizationUsage usage = VulkanRasterizationUsage::None;
		VulkanDepthBias bias{};

		VulkanRasterization(const Rasterization& other = {}) :usage(other.usage), bias(other.bias) {}

		operator Rasterization() const
		{
			return { usage, bias };
		}

		//TODO: get Vk things
	};

} // namespace np::gpu

#endif /* NP_ENGINE_GPU_VULKAN_RASTERIZATION_HPP */