//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/3/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_COMMAND_HPP
#define NP_ENGINE_VULKAN_COMMAND_HPP

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/Interface/Interface.hpp"

namespace np::gfx::__detail
{
	struct VulkanCommand : public Command
	{
		virtual void ApplyTo(VkCommandBuffer command_buffer) = 0;
	};
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_COMMAND_HPP */