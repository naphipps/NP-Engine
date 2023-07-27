//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 7/26/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_PHYSICAL_DEVICE_HPP
#define NP_ENGINE_VULKAN_PHYSICAL_DEVICE_HPP

#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

namespace np::gpu::__detail
{
	enum class VulkanPhysicalDeviceType : ui32
	{
		None = 0,
		Amd,
		Nvidia
		// TODO: add Intel maybe
	};

	struct VulkanPhysicalDevice
	{
		static VulkanPhysicalDeviceType GetType(VkPhysicalDevice physical_device)
		{
			VulkanPhysicalDeviceType type = VulkanPhysicalDeviceType::None;

			VkPhysicalDeviceProperties physical_properties;
			vkGetPhysicalDeviceProperties(physical_device, &physical_properties);

			str device_name = to_str(physical_properties.deviceName);
			for (siz i = 0; i < device_name.size(); i++)
				device_name[i] = ::std::toupper(device_name[i]);

			if (device_name.find("AMD") != -1) // TODO: how do we avoid using magic values here
			{
				type = VulkanPhysicalDeviceType::Amd;
			}
			else if (device_name.find("NVIDIA") != -1)
			{
				type = VulkanPhysicalDeviceType::Nvidia;
			}

			return type;
		}
	};
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_PHYSICAL_DEVICE_HPP */