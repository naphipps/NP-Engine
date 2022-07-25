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

#ifndef NP_ENGINE_VULKAN_MAX_FRAME_COUNT
	#define NP_ENGINE_VULKAN_MAX_FRAME_COUNT 2 //TODO: get to the point where we can just change this arbitarily .. I think we can do that?
#endif

#if NP_ENGINE_PLATFORM_IS_APPLE || NP_ENGINE_PLATFORM_IS_LINUX
	#define NP_ENGINE_PLATFORM_SUPPORTS_VULKAN_TIMELINE_SEMAPHORES false
#elif NP_ENGINE_PLATFORM_IS_WINDOWS
	#define NP_ENGINE_PLATFORM_SUPPORTS_VULKAN_TIMELINE_SEMAPHORES true
#endif

// TODO: add a bunch of helper functions like CreateSubmitInfo that sets "our default" SubmitInfo values
namespace np::graphics::rhi
{
	static bl VulkanHasStencilComponent(VkFormat format)
	{
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VENDOR_VULKAN_INCLUDE_HPP */