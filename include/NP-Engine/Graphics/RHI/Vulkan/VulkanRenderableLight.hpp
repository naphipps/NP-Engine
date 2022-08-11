//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDERABLE_LIGHT_HPP
#define NP_ENGINE_VULKAN_RENDERABLE_LIGHT_HPP

#include <iostream> //TODO: remove

#include "NP-Engine/Primitive/Primitive.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanRenderableImage.hpp"
#include "VulkanRenderableModel.hpp"

namespace np::graphics::rhi
{
	// TODO: implement this
	class VulkanRenderableLight : public RenderableLight
	{
	private:
		void RenderToFrame(VulkanFrame& frame, VulkanPipeline& pipeline, VulkanRenderableImage& image) {}

		void RenderToFrame(VulkanFrame& frame, VulkanPipeline& pipeline, VulkanRenderableModel& model) {}

		void Dispose() {}

	public:
		VulkanRenderableLight(Light& light): RenderableLight(light) {}

		~VulkanRenderableLight()
		{
			Dispose();
		}

		void RenderToFrame(Frame& frame, Pipeline& pipeline, RenderableObject& object) override
		{
			// TODO: implement this
			switch (object.GetType())
			{
			case RenderableType::Image:
				RenderToFrame((VulkanFrame&)frame, (VulkanPipeline&)pipeline, (VulkanRenderableImage&)object);
				break;
			case RenderableType::Model:
				RenderToFrame((VulkanFrame&)frame, (VulkanPipeline&)pipeline, (VulkanRenderableModel&)object);
				break;
			default:
				break;
			}
		}

		void PrepareForPipeline(Pipeline& pipeline) override {}

		void DisposeForPipeline(Pipeline& pipeline) override
		{
			// TODO: implement
			Dispose();
		}

		bl IsOutOfDate() const override
		{
			return true;
		}

		void SetOutOfDate(bl is_out_of_date = true) override {}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_RENDERABLE_LIGHT_HPP */