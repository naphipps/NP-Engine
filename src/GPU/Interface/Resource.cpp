//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/3/26
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/Resource.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanResource.hpp"

namespace np::gpu
{
	mem::sptr<ResourceLayout> ResourceLayout::Create(mem::sptr<Device> device, con::vector<ResourceDescription> descriptions)
	{
		mem::sptr<ResourceLayout> layout = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			layout =
				mem::create_sptr<__detail::VulkanResourceLayout>(device->GetServices()->GetAllocator(), device, descriptions);
			break;

		default:
			break;
		}

		return layout;
	}

	mem::sptr<ResourceGroupPool> ResourceGroupPool::Create(mem::sptr<Device> device, siz size,
														   con::vector<ResourceDescription> descriptions)
	{
		mem::sptr<ResourceGroupPool> pool = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			pool = mem::create_sptr<__detail::VulkanResourceGroupPool>(device->GetServices()->GetAllocator(), device, size,
																	   descriptions);
			break;

		default:
			break;
		}

		return pool;
	}
} // namespace np::gpu