//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/RenderPass.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanRenderPass.hpp"

namespace np::gpu
{
	mem::sptr<RenderPass> RenderPass::Create(mem::sptr<Device> device,
											 const con::vector<ImageResourceDescription>& descriptions,
											 const con::vector<SubpassDescription>& subpasses,
											 const con::vector<SubpassDependency>& dependencies)
	{
		mem::sptr<RenderPass> pass = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			pass = mem::create_sptr<__detail::VulkanRenderPass>(device->GetServices()->GetAllocator(), device, descriptions,
																subpasses, dependencies);
			break;

		default:
			break;
		}

		return pass;
	}
} // namespace np::gpu