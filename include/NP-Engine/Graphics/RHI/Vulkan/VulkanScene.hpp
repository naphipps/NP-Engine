//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/16/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VULKAN_SCENE_HPP
#define NP_ENGINE_VULKAN_SCENE_HPP

#include <iostream> //TODO: remove

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
		VulkanCamera _camera;
		concurrency::Thread _draw_loop_thread;
		atm_bl _draw_loop_procedure_is_complete;
		atm_bl _engage_draw_loop_procedure;
		atm_bl _drawing_is_complete;

		// TODO: WindowResizeCallback and WindowPositionCallback can be called very fast in succession - add a threshold for
		// when these actually draw so we don't get so bogged down with draw calls

		static void WindowResizeCallback(void* scene, ui32 width, ui32 height)
		{
			// TODO: find a way for these callbacks to trigger a thread that renders on a loop until Draw is called again
			VulkanScene& vulkan_scene = (VulkanScene&)*((VulkanScene*)scene);
			vulkan_scene.EngageDrawLoopProcedure();
		}

		static void WindowPositionCallback(void* scene, i32 x, i32 y)
		{
			// TODO: find a way for these callbacks to trigger a thread that renders on a loop until Draw is called again
			VulkanScene& vulkan_scene = (VulkanScene&)*((VulkanScene*)scene);
			vulkan_scene.EngageDrawLoopProcedure();
		}

		// TODO: investigate FramebufferResize callback for glfw

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

			// TODO: telling the renderer to draw may require the usage of Command objects...

			// TODO: loop through Entities to get camera, and objects to renderer, etc

			VulkanFrame& vulkan_frame = (VulkanFrame&)_renderer.BeginFrame();
			if (vulkan_frame.IsValid())
			{
				VulkanRenderer& vulkan_renderer = (VulkanRenderer&)_renderer;
				VulkanPipeline& object_pipeline = vulkan_renderer.GetObjectPipeline();
				VulkanPipeline& light_pipeline = vulkan_renderer.GetLightPipeline();


				UpdateCamera();
				UniformBufferObject ubo = object_pipeline.GetUbo();
				ubo.View = _camera.View;
				ubo.Projection = _camera.Projection;
				object_pipeline.SetUbo(ubo);
				light_pipeline.SetUbo(ubo);

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
					//TODO: we'll need to address the light assign for us to maximize performance here

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

		//TODO: I don't like how we have our camera api setup since we overwrite everything in UpdateCamera()
		void UpdateCamera()
		{
			_camera.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			_camera.Projection =
				glm::perspective(glm::radians(45.0f), ((VulkanRenderer&)_renderer).GetSwapchain().GetAspectRatio(), 0.1f, 10.0f);
			_camera.Projection[1][1] *= -1; // glm was made for OpenGL, so Y is inverted. We fix/invert Y here.
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
		}

		virtual void SetCamera(Camera& camera) override
		{
			_camera = camera;
		}

		virtual void AdjustForWindowResize(window::Window& window) override {}
	};
} // namespace np::graphics::rhi

#endif /* NP_ENGINE_VULKAN_SCENE_HPP */