//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_VIEWPORT_HPP
#define NP_ENGINE_GPU_VULKAN_VIEWPORT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Interface.hpp"

#include "VulkanDepthStencil.hpp"

namespace np::gpu::__detail
{
	struct VulkanViewport
	{
		::glm::vec2 position{};
		flt width = 0;
		flt height = 0;
		VulkanDepthBounds depthBounds{};

		VulkanViewport(const Viewport& other = {}):
			position(other.position),
			width(other.width),
			height(other.height),
			depthBounds(other.depthBounds)
		{}

		operator Viewport() const
		{
			return {position, width, height, depthBounds};
		}

		VkViewport GetVkViewport() const
		{
			return {position.x, position.y, width, height, depthBounds.min, depthBounds.max};
		}
	};
} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_VIEWPORT_HPP */