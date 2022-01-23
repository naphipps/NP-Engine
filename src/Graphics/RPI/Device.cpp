//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/21/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/RPI/Device.hpp" //main include

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanDevice.hpp" //because we're in cpp

/*
namespace np::graphics
{
	Device* Device::Create(memory::Allocator& allocator)
	{
		Device* device = nullptr;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			memory::Block block = allocator.Allocate(sizeof(rhi::VulkanDevice));
			memory::Construct<rhi::VulkanDevice>(block, allocator);
			device = (Device*)block.Begin(); //TODO: figure out if we're going to use ptr or Begin...
			break;
		}

		return device;
	}
}
*/