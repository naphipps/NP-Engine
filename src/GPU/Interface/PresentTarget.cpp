//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/PresentTarget.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanPresentTarget.hpp"

namespace np::gpu
{
	mem::sptr<PresentTarget> PresentTarget::Create(mem::sptr<DetailInstance> instance, mem::sptr<win::Window> window)
	{
		mem::sptr<PresentTarget> target = nullptr;

		switch (instance->GetDetailType())
		{
		case DetailType::Vulkan:
			target = mem::create_sptr<__detail::VulkanPresentTarget>(instance->GetServices()->GetAllocator(), instance, window);
			break;

		default:
			break;
		}

		return target;
	}
} // namespace np::gpu