//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/16/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SCENE_HPP
#define NP_ENGINE_VULKAN_SCENE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"
#include "NP-Engine/Time/Time.hpp"

#include "NP-Engine/Vendor/VulkanInclude.hpp"

#include "NP-Engine/Graphics/RPI/RPI.hpp"

#include "VulkanRenderer.hpp"
#include "VulkanFrame.hpp"
#include "VulkanRenderableModel.hpp"
#include "VulkanCamera.hpp"

namespace np::graphics::rhi
{
	class VulkanScene : public Scene
	{
	private:
		// TODO: I think our scene needs it's own ecs registry

		VulkanCamera _camera;
		concurrency::Thread _draw_loop_thread;
		atm_bl _draw_loop_procedure_is_complete;
		atm_bl _engage_draw_loop_procedure;
		atm_bl _drawing_is_complete;

		static void WindowResizeCallback(void* scene, ui32 width, ui32 height)
		{
			VulkanScene& vulkan_scene = (VulkanScene&)*((VulkanScene*)scene);
			vulkan_scene.EngageDrawLoopProcedure();
		}

		static void WindowPositionCallback(void* scene, i32 x, i32 y)
		{
			VulkanScene& vulkan_scene = (VulkanScene&)*((VulkanScene*)scene);
			vulkan_scene.EngageDrawLoopProcedure();
		}

		void EngageDrawLoopProcedure()
		{
			bl expected = false;
			if (_engage_draw_loop_procedure.compare_exchange_strong(expected, true, mo_release, mo_relaxed))
			{
				_draw_loop_procedure_is_complete.store(false, mo_release);
				_draw_loop_thread.Dispose();
				_draw_loop_thread.Run(&VulkanScene::DrawLoopProcedure, this);
			}
		}

		void DrawLoopProcedure()
		{
			time::SteadyTimestamp prev = time::SteadyClock::now();
			time::SteadyTimestamp next;

			siz max_duration = NP_ENGINE_APPLICATION_LOOP_DURATION;

			while (_engage_draw_loop_procedure.load(mo_acquire))
			{
				((VulkanRenderer&)_renderer).SetOutOfDate();

				Draw();

				next = time::SteadyClock::now();
				time::DurationMilliseconds duration = next - prev;
				prev = next;
				if (duration.count() < max_duration)
				{
					concurrency::ThisThread::sleep_for(time::DurationMilliseconds(max_duration - duration.count()));
				}
			}

			_draw_loop_procedure_is_complete.store(true, mo_release);
		}

		void Draw()
		{
			bl expected = true;
			while (!_drawing_is_complete.compare_exchange_weak(expected, false, mo_release, mo_relaxed))
			{
				expected = true;
			}

			_on_draw_delegate.InvokeConnectedFunction();
			VulkanFrame& vulkan_frame = (VulkanFrame&)_renderer.BeginFrame();
			if (vulkan_frame.IsValid())
			{
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
				auto object_entities = _ecs_registry.view<RenderableObject*>();
				for (auto e : object_entities)
				{
					RenderableObject& object = *ecs::Entity(e, _ecs_registry).Get<RenderableObject*>();
					object.RenderToFrame(vulkan_frame, object_pipeline);
				}
				object_pipeline.BindDescriptorSetsToFrame(vulkan_frame);

				light_pipeline.BindPipelineToFrame(vulkan_frame);
				light_pipeline.PrepareToBindDescriptorSets(vulkan_frame);
				auto light_entities = _ecs_registry.view<RenderableLight*>();
				for (auto e : light_entities)
				{
					// TODO: we'll need to address the light assign for us to maximize performance here

					RenderableLight& light = *ecs::Entity(e, _ecs_registry).Get<RenderableLight*>();
					for (auto e : object_entities)
					{
						RenderableObject& object = *ecs::Entity(e, _ecs_registry).Get<RenderableObject*>();
						light.RenderToFrame(vulkan_frame, light_pipeline, object);
					}
				}
				light_pipeline.BindDescriptorSetsToFrame(vulkan_frame);

				vulkan_renderer.EndRenderPassOnFrame(vulkan_frame);
				vulkan_frame.SubmitStagedCommandsToBuffer();

				_renderer.EndFrame();
				_renderer.DrawFrame();
			}

			_drawing_is_complete.store(true, mo_release);
		}

	public:
		VulkanScene(::entt::registry& ecs_registry, Renderer& renderer):
			Scene(ecs_registry, renderer),
			_draw_loop_procedure_is_complete(true),
			_engage_draw_loop_procedure(false),
			_drawing_is_complete(true)
		{
			((VulkanRenderer&)_renderer).GetSurface().GetWindow().SetResizeCallback(this, WindowResizeCallback);
			((VulkanRenderer&)_renderer).GetSurface().GetWindow().SetPositionCallback(this, WindowPositionCallback);
		}

		~VulkanScene()
		{
			Dispose();
		}

		void UpdateCamera()
		{
			_camera.AspectRatio = ((VulkanRenderer&)_renderer).GetSwapchain().GetAspectRatio();
			_camera.Update();
		}

		void Render() override
		{
			while (!_draw_loop_procedure_is_complete.load(mo_acquire))
				_engage_draw_loop_procedure.store(false, mo_release);

			Draw();
		}

		void Prepare() override
		{
			auto object_entities = _ecs_registry.view<RenderableObject*>();
			for (auto e : object_entities)
			{
				RenderableObject& object = *ecs::Entity(e, _ecs_registry).Get<RenderableObject*>();
				object.PrepareForPipeline(((VulkanRenderer&)_renderer).GetObjectPipeline());
			}

			auto light_entities = _ecs_registry.view<RenderableLight*>();
			for (auto e : light_entities)
			{
				RenderableLight& light = *ecs::Entity(e, _ecs_registry).Get<RenderableLight*>();
				light.PrepareForPipeline(((VulkanRenderer&)_renderer).GetLightPipeline());
			}
		}

		void Dispose() override
		{
			auto object_entities = _ecs_registry.view<RenderableObject*>();
			for (auto e : object_entities)
			{
				RenderableObject& object = *ecs::Entity(e, _ecs_registry).Get<RenderableObject*>();
				object.DisposeForPipeline(((VulkanRenderer&)_renderer).GetObjectPipeline());
			}

			auto light_entities = _ecs_registry.view<RenderableLight*>();
			for (auto e : light_entities)
			{
				RenderableLight& light = *ecs::Entity(e, _ecs_registry).Get<RenderableLight*>();
				light.DisposeForPipeline(((VulkanRenderer&)_renderer).GetLightPipeline());
			}

			_on_draw_delegate.DisconnectFunction();
		}

		virtual void SetCamera(Camera& camera) override
		{
			_camera = camera;
		}

		virtual void AdjustForWindowResize(window::Window& window) override {}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SCENE_HPP */