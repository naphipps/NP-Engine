//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_HPP
#define NP_ENGINE_VULKAN_COMMAND_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

namespace np::graphics::rhi
{
	struct VulkanCommand : public Command
	{
		virtual void ApplyTo(VkCommandBuffer command_buffer) = 0;
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_COMMAND_HPP */