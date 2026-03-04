//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/7/26
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/ImageResourceView.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanImageResourceView.hpp"

namespace np::gpu
{
	mem::sptr<ImageResourceView> ImageResourceView::Create(mem::sptr<Device> device, mem::sptr<ImageResource> image, ImageResourceUsage usage)
	{
		mem::sptr<ImageResourceView> view = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			view = mem::create_sptr<__detail::VulkanImageResourceView>(device->GetServices()->GetAllocator(), device, image, usage);
			break;
		}

		return view;
	}
} // namespace np::gpu