//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_GRAPHICS_PIPELINE_HPP
#define NP_ENGINE_GPU_INTERFACE_GRAPHICS_PIPELINE_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Detail.hpp"
#include "Pipeline.hpp"
#include "Shader.hpp"
#include "RenderPass.hpp"
#include "Topology.hpp"
#include "Viewport.hpp"
#include "Scissor.hpp"
#include "Rasterization.hpp"
#include "Logic.hpp"
#include "Blend.hpp"
#include "Dynamic.hpp"
#include "DepthStencil.hpp"
#include "Multisample.hpp"
#include "PipelineCache.hpp"
#include "Format.hpp"

namespace np::gpu
{
	struct GraphicsPipeline : public Pipeline
	{
		static mem::sptr<GraphicsPipeline> Create(mem::sptr<RenderPass> render_pass,
			PipelineUsage usage,
			mem::sptr<PipelineResourceLayout> layout,
			const con::vector<mem::sptr<Shader>>& shaders,
			const con::vector<Format>& input_vertex_formatting,
			const con::vector<Format>& input_instance_formatting,
			PrimitiveTopology topology,
			siz tessellation_patch_control_point_count,
			const con::vector<Viewport>& viewports,
			const con::vector<Scissor>& scissors,
			const Rasterization& rasterization,
			const Multisample& multisample,
			const DepthStencil& depth_stencil,
			const Blend& blend,
			DynamicUsage dynamic_usage,
			mem::sptr<PipelineCache> cache);

		virtual ~GraphicsPipeline() = default;

		virtual PipelineType GetPipelineType() const override
		{
			return PipelineType::Graphics;
		}

		//TODO: add getters for everything in the Create function
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_GRAPHICS_PIPELINE_HPP */