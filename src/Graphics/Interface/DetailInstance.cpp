//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Graphics/Interface/DetailInstance.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/Graphics/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/Graphics/Detail/Vulkan/VulkanInstance.hpp"

namespace np::gfx
{
	mem::sptr<DetailInstance> DetailInstance::Create(GraphicsDetailType detail_type, mem::sptr<srvc::Services> services)
	{
		mem::sptr<DetailInstance> instance = nullptr;

		switch (detail_type)
		{
		case GraphicsDetailType::Vulkan:
			instance = mem::create_sptr<__detail::VulkanInstance>(services->GetAllocator(), services);
			break;

		default:
			break;
		}

		return instance;
	}
}