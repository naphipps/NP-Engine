//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/21/21
//
//##===----------------------------------------------------------------------===##//

//TODO: I think we can remove this file

#include "NP-Engine/Graphics/RPI/Device.hpp" //main include

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanDevice.hpp" //because we're in cpp

/*
namespace np::gfx
{
	Device* Device::Create(mem::Allocator& allocator)
	{
		Device* device = nullptr;

		switch (__detail::RegisteredRhiType.load(mo_acquire))
		{
		case RhiType::Vulkan:
			mem::Block block = allocator.Allocate(sizeof(rhi::VulkanDevice));
			mem::Construct<rhi::VulkanDevice>(block, allocator);
			device = (Device*)block.Begin(); //TODO: figure out if we're going to use ptr or Begin...
			break;
		}

		return device;
	}
}
*/