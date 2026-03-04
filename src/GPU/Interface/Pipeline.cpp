//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/Pipeline.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanPipeline.hpp"

namespace np::gpu
{
	mem::sptr<PipelineResourceLayout> PipelineResourceLayout::Create(mem::sptr<Device> device,
		const con::vector<mem::sptr<ResourceLayout>>& resource_layouts, PushData push_data)
	{
		mem::sptr<PipelineResourceLayout> layout = nullptr;

		switch (device->GetDetailType())
		{
		case DetailType::Vulkan:
			layout = mem::create_sptr<__detail::VulkanPipelineResourceLayout>(device->GetServices()->GetAllocator(), device, resource_layouts, push_data);
			break;

		default:
			break;
		}

		return layout;
	}
} // namespace np::gpu