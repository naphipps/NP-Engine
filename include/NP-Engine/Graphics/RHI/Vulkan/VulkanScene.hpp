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

namespace np::gfx::rhi
{
	class VulkanScene : public Scene
	{
	private:
		// TODO: I think our scene needs it's own ecs registry!! //TODO: put this in RPI

		VulkanCamera _camera;
		thr::Thread _dlp_thread; // dlp = drawing loop procedure
		atm_bl _dlp_is_complete;
		atm_bl _dlp_keep_alive;
		atm_bl _dlp_enable_draw;
		atm_bl _is_drawing;

		static void WindowResizeCallback(void* scene, ui32 width, ui32 height)
		{
			VulkanScene& vulkan_scene = (VulkanScene&)*((VulkanScene*)scene);
			vulkan_scene._dlp_enable_draw.store(true, mo_release);
		}

		static void WindowPositionCallback(void* scene, i32 x, i32 y)
		{
			VulkanScene& vulkan_scene = (VulkanScene&)*((VulkanScene*)scene);
			vulkan_scene._dlp_enable_draw.store(true, mo_release);
		}

		void DrawLoopProcedure()
		{
			NP_ENGINE_PROFILE_SCOPE("VulkanScene::DrawLoopProcedure " + to_str((ui64)this));

			tim::SteadyTimestamp next = tim::SteadyClock::now();
			tim::SteadyTimestamp prev = next;
			const tim::DblMilliseconds min_duration(NP_ENGINE_APPLICATION_LOOP_DURATION);
			VulkanRenderer& vulkan_renderer = (VulkanRenderer&)_renderer;

			while (_dlp_keep_alive.load(mo_acquire))
			{
				vulkan_renderer.SetOutOfDate();

				if (_dlp_enable_draw.load(mo_acquire))
				{
					Draw();
					for (next = tim::SteadyClock::now(); next - prev < min_duration; next = tim::SteadyClock::now())
						thr::ThisThread::yield();

					prev = next;
				}
				else
				{
					thr::ThisThread::sleep_for(min_duration);
				}
			}

			_dlp_is_complete.store(true, mo_release);
		}

		void Draw()
		{
			NP_ENGINE_PROFILE_SCOPE("vulkan scene draw");
			bl expected = false;
			while (!_is_drawing.compare_exchange_weak(expected, true, mo_release, mo_relaxed))
				expected = false;

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

			_is_drawing.store(false, mo_release);
		}

	public:
		VulkanScene(srvc::Services& services, Renderer& renderer):
			Scene(services, renderer),
			_dlp_is_complete(true),
			_dlp_keep_alive(false),
			_dlp_enable_draw(false),
			_is_drawing(false)
		{
			((VulkanRenderer&)_renderer).GetSurface().GetWindow().SetResizeCallback(this, WindowResizeCallback);
			((VulkanRenderer&)_renderer).GetSurface().GetWindow().SetPositionCallback(this, WindowPositionCallback);

			_dlp_keep_alive.store(true, mo_release);
			_dlp_is_complete.store(false, mo_release);
			_dlp_thread.Run(&VulkanScene::DrawLoopProcedure, this);
			_dlp_thread.SetAffinity(1);
		}

		~VulkanScene()
		{
			while (!_dlp_is_complete.load(mo_acquire))
				_dlp_keep_alive.store(false, mo_release);

			_dlp_thread.Dispose();

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
			_dlp_enable_draw.store(false, mo_release);
			Draw();
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
			_dlp_enable_draw.store(false, mo_release);
			while (_is_drawing.load(mo_acquire))
				thr::ThisThread::yield();

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