//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 6/8/26
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/ResourceGroup.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanResourceGroup.hpp"

namespace np::gpu
{
	mem::sptr<ResourceGroupPool> ResourceGroupPool::Create(mem::sptr<Device> device, siz size,
		const con::vector<ResourceDescription>& descriptions)
	{
		mem::sptr<ResourceGroupPool> pool = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			pool = mem::create_sptr<__detail::VulkanResourceGroupPool>(device->GetServices()->GetAllocator(), device, size, descriptions);
			break;

		default:
			break;
		}

		return pool;
	}
} // namespace np::gpu