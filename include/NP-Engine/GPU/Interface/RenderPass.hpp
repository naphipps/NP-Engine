//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/12/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_RENDERPASS_HPP
#define NP_ENGINE_GPU_INTERFACE_RENDERPASS_HPP

#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "Access.hpp"
#include "Device.hpp"
#include "ImageResource.hpp"
#include "Stage.hpp"

namespace np::gpu
{
	//TODO: if subpass is indeed only renderpass related, then consider renaming subpass to renderstep

	struct SubpassImageResourceReference
	{
		siz framebufferImageViewIndex = SIZ_MAX;
		ImageResourceUsage usage = ImageResourceUsage::None; //the expected usage/layout of the image for this subpass
	};

	struct SubpassDescription
	{
		con::vector<SubpassImageResourceReference> inputs{}; //input images
		con::vector<SubpassImageResourceReference> outputs{}; //output images
		con::vector<SubpassImageResourceReference>
			multisampleResolves{}; //multisampled images whose results are to be resolved over subpass
		con::vector<siz> preserveFramebufferImageViewIndicies{}; //preserve these images over subpass
	};

	struct SubpassDependency
	{
		siz srcSubpassIndex = 0; //producer
		siz dstSubpassIndex = 0; //consumer
		Stage srcStage = Stage::None; //stages we produce
		Stage dstStage = Stage::None; //stages we consume
		Access srcAccess = Access::None; //how producer subpass will access attachments
		Access dstAccess = Access::None; //how consumer subpass will access attachments
	};

	class SubpassUsage : public enm_ui32
	{
	public:
		constexpr static ui32 HasSecondary = BIT(0);

		SubpassUsage(ui32 value): enm_ui32(value) {}
	};

	struct RenderPass : public DetailObject //TODO: rename to "Renderpass"
	{
		static mem::sptr<RenderPass> Create(mem::sptr<Device> device, const con::vector<ImageResourceDescription>& descriptions,
											const con::vector<SubpassDescription>& subpasses,
											const con::vector<SubpassDependency>& dependencies);

		virtual ~RenderPass() = default;

		virtual mem::sptr<Device> GetDevice() const = 0;

		virtual con::vector<ImageResourceDescription> GetImageResourceDescriptions() const = 0;

		virtual con::vector<SubpassDescription> GetSubpassDescriptions() const = 0;

		virtual con::vector<SubpassDependency> GetSubpassDepenedencies() const = 0;
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_RENDERPASS_HPP */