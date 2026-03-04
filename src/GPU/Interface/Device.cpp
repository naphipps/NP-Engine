//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/Device.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanDevice.hpp"

namespace np::gpu
{
	mem::sptr<Device> Device::Create(mem::sptr<DetailInstance> instance, DeviceUsage usage, mem::sptr<PresentTarget> target)
	{
		usage |=
			target ? DeviceUsage::Present : DeviceUsage::None; //enforce Present usage when creating device with PresentTarget
		mem::sptr<Device> device = nullptr;

		switch (instance->GetDetailType())
		{
		case DetailType::Vulkan:
			device = mem::create_sptr<__detail::VulkanDevice>(instance->GetServices()->GetAllocator(), instance, usage, target);
			break;

		default:
			break;
		}

		return device;
	}
} // namespace np::gpu