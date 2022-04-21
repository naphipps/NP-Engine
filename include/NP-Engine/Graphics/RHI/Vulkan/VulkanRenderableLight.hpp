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

namespace np::graphics::rhi
{
	class VulkanRenderableLight : public RenderableLight
	{
	public:
		VulkanRenderableLight(Light& light): RenderableLight(light)
		{
			// TODO: implement this
		}

		~VulkanRenderableLight()
		{
			::std::cout << "loud VulkanRenderableLight destructor!\n";
		}

		void RenderToFrame(Frame& frame, Pipeline& pipeline) override
		{
			// TODO: implement this
		}

		void PrepareForPipeline(Pipeline& pipeline) override {}

		void DisposeForPipeline(Pipeline& pipeline) override
		{
			//TODO: implement
		}

		bl IsOutOfDate() const override
		{
			return true;
		}

		void SetOutOfDate(bl is_out_of_date = true) override {}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_RENDERABLE_LIGHT_HPP */