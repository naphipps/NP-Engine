//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 4/8/23
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/GPU/Interface/GraphicsPipeline.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/GPU/Detail/OpenGL/OpenGLGraphics.hpp"
#endif

#include "NP-Engine/GPU/Detail/Vulkan/VulkanGraphicsPipeline.hpp"

namespace np::gpu
{
	mem::sptr<GraphicsPipeline> GraphicsPipeline::Create(
		mem::sptr<RenderPass> render_pass, PipelineUsage usage, mem::sptr<PipelineResourceLayout> layout,
		const con::vector<mem::sptr<Shader>>& shaders, const con::vector<Format>& input_vertex_formatting,
		const con::vector<Format>& input_instance_formatting, PrimitiveTopology topology,
		siz tessellation_patch_control_point_count, const con::vector<Viewport>& viewports,
		const con::vector<Scissor>& scissors, const Rasterization& rasterization, const Multisample& multisample,
		const DepthStencil& depth_stencil, const Blend& blend, DynamicUsage dynamic_usage, mem::sptr<PipelineCache> cache)
	{
		mem::sptr<GraphicsPipeline> pipeline = nullptr;

		switch (render_pass->GetDetailType())
		{
		case DetailType::Vulkan:
			pipeline = mem::create_sptr<__detail::VulkanGraphicsPipeline>(
				render_pass->GetServices()->GetAllocator(), render_pass, usage, layout, shaders, input_vertex_formatting,
				input_instance_formatting, topology, tessellation_patch_control_point_count, viewports, scissors, rasterization,
				multisample, depth_stencil, blend, dynamic_usage, cache);
			break;

		default:
			break;
		}

		return pipeline;
	}
} // namespace np::gpu