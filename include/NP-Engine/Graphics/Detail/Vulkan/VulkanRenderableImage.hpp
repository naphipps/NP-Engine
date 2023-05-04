//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDERABLE_IMAGE_HPP
#define NP_ENGINE_VULKAN_RENDERABLE_IMAGE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Graphics/Interface/Interface.hpp"

namespace np::gpu::__detail
{
	class VulkanRenderableImage : public RenderableImage
	{
	private:
		void Dispose() {}

	public:
		VulkanRenderableImage(mem::sptr<srvc::Services> services, mem::sptr<Image> image): RenderableImage(services, image) {}

		~VulkanRenderableImage()
		{
			Dispose();
		}

		// TODO: add copy/move stuff??

		void Stage(mem::sptr<CommandStaging> frame, mem::sptr<Pipeline> pipeline) override
		{
			// TODO: implement this
		}

		void PrepareForPipeline(mem::sptr<Pipeline> pipeline) override
		{
			// TODO: implement this
		}

		void DisposeForPipeline(mem::sptr<Pipeline> pipeline) override
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
} // namespace np::gpu::__detail

#endif /* NP_ENGINE_VULKAN_RENDERABLE_IMAGE_HPP */