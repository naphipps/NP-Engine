//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/11/26
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_VULKAN_SCISSOR_HPP
#define NP_ENGINE_GPU_VULKAN_SCISSOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/GPU/Interface/Scissor.hpp"

namespace np::gpu::__detail
{
	struct VulkanScissor
	{
		::glm::i32vec2 offset{0};
		ui32 width = 0;
		ui32 height = 0;

		VulkanScissor(const Scissor& other = {}): offset{other.offset}, width(other.width), height(other.height) {}

		operator Scissor() const
		{
			return {offset, width, height};
		}

		VkRect2D GetVkRect2D() const
		{
			return {{offset.x, offset.y}, {width, height}};
		}
	};

} //namespace np::gpu::__detail

#endif /* NP_ENGINE_GPU_VULKAN_SCISSOR_HPP */