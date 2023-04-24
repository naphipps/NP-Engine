//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/15/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_RENDERABLE_LIGHT_HPP
#define NP_ENGINE_VULKAN_RENDERABLE_LIGHT_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Graphics/Interface/Interface.hpp"

#include "VulkanRenderableImage.hpp"
#include "VulkanRenderableModel.hpp"

namespace np::gfx::__detail
{
	// TODO: implement this
	class VulkanRenderableLight : public RenderableLight
	{
	private:
		void Stage(mem::sptr<CommandStaging> staging, mem::sptr<Pipeline> pipeline, VulkanRenderableImage& image) {}

		void Stage(mem::sptr<CommandStaging> staging, mem::sptr<Pipeline> pipeline, VulkanRenderableModel& model) {}

		void Dispose()
		{
			//TODO: implement
		}

	public:
		VulkanRenderableLight(mem::sptr<srvc::Services> services, mem::sptr<Light> light): RenderableLight(services, light) {}

		~VulkanRenderableLight()
		{
			Dispose();
		}

		void Stage(mem::sptr<CommandStaging> staging, mem::sptr<Pipeline> pipeline, mem::sptr<Resource> resource) override
		{
			// TODO: implement this
			switch (resource->GetType())
			{
			case ResourceType::RenderableImage:
				Stage(staging, pipeline, (VulkanRenderableImage&)(*resource));
				break;

			case ResourceType::RenderableModel:
				Stage(staging, pipeline, (VulkanRenderableModel&)(*resource));
				break;

			default:
				break;
			}
		}

		void PrepareForPipeline(mem::sptr<Pipeline> pipeline) override {}

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
} // namespace np::gfx::__detail

#endif /* NP_ENGINE_VULKAN_RENDERABLE_LIGHT_HPP */