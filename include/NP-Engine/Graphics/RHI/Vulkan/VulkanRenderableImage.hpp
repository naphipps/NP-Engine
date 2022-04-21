//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDERABLE_IMAGE_HPP
#define NP_ENGINE_VULKAN_RENDERABLE_IMAGE_HPP

#include <iostream> //TODO: remove

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

namespace np::graphics::rhi
{
	class VulkanRenderableImage : public RenderableImage
	{
	public:
		VulkanRenderableImage(Image& image): RenderableImage(image) {}

		~VulkanRenderableImage()
		{
			::std::cout << "loud VulkanRenderableImage destructor\n";
		}

		// TODO: add copy/move stuff??

		void RenderToFrame(Frame& frame, Pipeline& pipeline) override
		{
			// TODO: implement this
		}

		void PrepareForPipeline(Pipeline& pipeline) override
		{
			// TODO: implement this
		}

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

#endif /* NP_ENGINE_VULKAN_RENDERABLE_IMAGE_HPP */