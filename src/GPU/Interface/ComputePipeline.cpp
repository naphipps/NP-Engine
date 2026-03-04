//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/1/26
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/ComputePipeline.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanComputePipeline.hpp"

namespace np::gpu
{
	mem::sptr<ComputePipeline> ComputePipeline::Create(mem::sptr<Shader> shader, mem::sptr<PipelineResourceLayout> layout,
													   PipelineUsage usage, mem::sptr<PipelineCache> cache)
	{
		mem::sptr<ComputePipeline> pipeline = nullptr;

		switch (shader->GetDetailType())
		{
		case DetailType::Vulkan:
			pipeline = mem::create_sptr<__detail::VulkanComputePipeline>(shader->GetServices()->GetAllocator(), shader, layout,
																		 usage, cache);
			break;

		default:
			break;
		}

		return pipeline;
	}
} // namespace np::gpu