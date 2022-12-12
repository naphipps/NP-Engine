//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/16/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SCENE_HPP
#define NP_ENGINE_VULKAN_SCENE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanRenderer.hpp"
#include "VulkanFrame.hpp"
#include "VulkanRenderableModel.hpp"
#include "VulkanCamera.hpp"

/*
	TODO: idea: make scenes accept a ptr to a registry, and it is nullptr, then use the registry in the services???
*/

namespace np::gfx::rhi
{
	class VulkanScene : public Scene
	{
	private:
		// TODO: I think our scene needs it's own ecs registry?? //TODO: put this in RPI

		VulkanCamera _camera;

	public:
		VulkanScene(srvc::Services& services, Renderer& renderer):
			Scene(services, renderer)
		{}

		~VulkanScene()
		{
			Dispose();
		}

		void UpdateCamera()
		{
			VulkanRenderer& vulkan_renderer = (VulkanRenderer&)_renderer;
			VkExtent2D extent = vulkan_renderer.GetSwapchain().GetExtent();

			_camera.AspectRatio = vulkan_renderer.GetSwapchain().GetAspectRatio();
			_camera.LeftPlane = -(flt)extent.width / 2.f;
			_camera.RightPlane = (flt)extent.width / 2.f;
			_camera.BottomPlane = -(flt)extent.height / 2.f;
			_camera.TopPlane = (flt)extent.height / 2.f;
			_camera.Update();
		}

		void Render() override
		{
			NP_ENGINE_PROFILE_SCOPE("vulkan scene draw");

			_on_draw_delegate.InvokeConnectedFunction();
			VulkanFrame& vulkan_frame = (VulkanFrame&)_renderer.BeginFrame();
			if (vulkan_frame.IsValid())
			{
				ecs::Registry& ecs_registry = _services.GetEcsRegistry();
				VulkanRenderer& vulkan_renderer = (VulkanRenderer&)_renderer;
				VulkanPipeline& object_pipeline = vulkan_renderer.GetObjectPipeline();
				VulkanPipeline& light_pipeline = vulkan_renderer.GetLightPipeline();

				UpdateCamera();
				PipelineMetaValues pipeline_meta_values = object_pipeline.GetMetaValues();
				pipeline_meta_values.View = _camera.GetView();
				pipeline_meta_values.Projection = _camera.GetProjection();
				object_pipeline.SetMetaValues(pipeline_meta_values);
				light_pipeline.SetMetaValues(pipeline_meta_values);

				vulkan_renderer.BeginRenderPassOnFrame(vulkan_frame);

				object_pipeline.BindPipelineToFrame(vulkan_frame);
				object_pipeline.PrepareToBindDescriptorSets(vulkan_frame);
				auto object_entities = ecs_registry.view<RenderableObject*>();
				for (auto e : object_entities)
				{
					RenderableObject& object = *ecs::Entity::Get<RenderableObject*>(e, ecs_registry);
					object.RenderToFrame(vulkan_frame, object_pipeline);
				}
				object_pipeline.BindDescriptorSetsToFrame(vulkan_frame);

				light_pipeline.BindPipelineToFrame(vulkan_frame);
				light_pipeline.PrepareToBindDescriptorSets(vulkan_frame);
				auto light_entities = ecs_registry.view<RenderableLight*>();
				for (auto e : light_entities)
				{
					// TODO: we'll need to address the light assign for us to maximize performance here

					RenderableLight& light = *ecs::Entity::Get<RenderableLight*>(e, ecs_registry);
					for (auto e : object_entities)
					{
						RenderableObject& object = *ecs::Entity::Get<RenderableObject*>(e, ecs_registry);
						light.RenderToFrame(vulkan_frame, light_pipeline, object);
					}
				}
				light_pipeline.BindDescriptorSetsToFrame(vulkan_frame);

				vulkan_renderer.EndRenderPassOnFrame(vulkan_frame);
				vulkan_frame.SubmitStagedCommandsToBuffer();

				_renderer.EndFrame();
				_renderer.DrawFrame();
			}
		}

		void Prepare() override
		{
			ecs::Registry& ecs_registry = _services.GetEcsRegistry();

			auto object_entities = ecs_registry.view<RenderableObject*>();
			for (auto e : object_entities)
			{
				RenderableObject& object = *ecs::Entity::Get<RenderableObject*>(e, ecs_registry);
				object.PrepareForPipeline(((VulkanRenderer&)_renderer).GetObjectPipeline());
			}

			auto light_entities = ecs_registry.view<RenderableLight*>();
			for (auto e : light_entities)
			{
				RenderableLight& light = *ecs::Entity::Get<RenderableLight*>(e, ecs_registry);
				light.PrepareForPipeline(((VulkanRenderer&)_renderer).GetLightPipeline());
			}
		}

		void Dispose() override
		{
			ecs::Registry& ecs_registry = _services.GetEcsRegistry();

			auto object_entities = ecs_registry.view<RenderableObject*>();
			for (auto e : object_entities)
			{
				RenderableObject& object = *ecs::Entity::Get<RenderableObject*>(e, ecs_registry);
				object.DisposeForPipeline(((VulkanRenderer&)_renderer).GetObjectPipeline());
			}

			auto light_entities = ecs_registry.view<RenderableLight*>();
			for (auto e : light_entities)
			{
				RenderableLight& light = *ecs::Entity::Get<RenderableLight*>(e, ecs_registry);
				light.DisposeForPipeline(((VulkanRenderer&)_renderer).GetLightPipeline());
			}

			_on_draw_delegate.DisconnectFunction();
		}

		virtual void SetCamera(Camera& camera) override
		{
			_camera = camera;
		}

		virtual void AdjustForWindowResize(win::Window& window) override {}
	};
} // namespace np::gfx::rhi

#endif /* NP_ENGINE_VULKAN_SCENE_HPP */