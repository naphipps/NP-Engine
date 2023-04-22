//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/5/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VENDOR_VULKAN_INCLUDE_HPP
#define NP_ENGINE_VENDOR_VULKAN_INCLUDE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include <vulkan/vulkan.hpp>

//#ifndef NP_ENGINE_VULKAN_MAX_FRAME_COUNT
	//#define NP_ENGINE_VULKAN_MAX_FRAME_COUNT 3
	// TODO: ^ get to the point where we can just change this arbitarily .. I think we can do that?
//#endif

// TODO: add a bunch of helper functions like CreateSubmitInfo that sets "our default" SubmitInfo values
namespace np::gfx::__detail
{
	static bl VulkanHasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VENDOR_VULKAN_INCLUDE_HPP */