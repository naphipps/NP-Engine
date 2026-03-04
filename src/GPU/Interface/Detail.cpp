//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/Detail.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanInstance.hpp"

namespace np::gpu
{
	mem::sptr<DetailInstance> DetailInstance::Create(DetailType type, mem::sptr<srvc::Services> services)
	{
		mem::sptr<DetailInstance> instance = nullptr;

		switch (type)
		{
		case DetailType::Vulkan:
			instance = mem::create_sptr<__detail::VulkanInstance>(services->GetAllocator(), services);
			break;

		default:
			break;
		}

		return instance;
	}
} // namespace np::gpu