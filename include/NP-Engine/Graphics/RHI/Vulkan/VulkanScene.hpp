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
		time::SteadyTimestamp _start_timestamp;

		// TODO: WindowResizeCallback and WindowPositionCallback can be called very fast in succession - add a threshold for
		// when these actually draw so we don't get so bogged down with draw calls

		static void WindowResizeCallback(void* scene, ui32 width, ui32 height)
		{
			// TODO: find a way for these callbacks to trigger a thread that renders on a loop until Draw is called again
			VulkanScene& vulkan_scene = (VulkanScene&)*((VulkanScene*)scene);
			VulkanRenderer& vulkan_renderer = (VulkanRenderer&)vulkan_scene.GetRenderer();
			vulkan_renderer.SetOutOfDate();
			vulkan_scene.Draw();
		}

		static void WindowPositionCallback(void* scene, i32 x, i32 y)
		{
			// TODO: find a way for these callbacks to trigger a thread that renders on a loop until Draw is called again
			VulkanScene& vulkan_scene = (VulkanScene&)*((VulkanScene*)scene);
			VulkanRenderer& vulkan_renderer = (VulkanRenderer&)vulkan_scene.GetRenderer();
			vulkan_renderer.SetOutOfDate();
			vulkan_scene.Draw();
		}

		// TODO: investigate FramebufferResize callback for glfw

	public:
		VulkanScene(::entt::registry& ecs_registry, Renderer& renderer): Scene(ecs_registry, renderer)
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
			_camera.View = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			_camera.Projection =
				glm::perspective(glm::radians(45.0f), ((VulkanRenderer&)_renderer).GetDevice().GetAspectRatio(), 0.1f, 10.0f);
			_camera.Projection[1][1] *= -1; // glm was made for OpenGL, so Y is inverted. We fix/invert Y here.
		}

		void Draw() override
		{
			// TODO: telling the renderer to draw may require the usage of Command objects...

			// TODO: loop through Entities to get camera, and objects to renderer, etc

			VulkanFrame& vulkan_frame = (VulkanFrame&)_renderer.BeginFrame();
			if (vulkan_frame.IsValid())
			{
				VulkanRenderer& vulkan_renderer = (VulkanRenderer&)_renderer;

				UpdateCamera();
				UniformBufferObject ubo = vulkan_renderer.GetObjectPipeline().GetUbo();
				ubo.View = _camera.View;
				ubo.Projection = _camera.Projection;
				vulkan_renderer.GetObjectPipeline().SetUbo(ubo);

				vulkan_renderer.BeginRenderPassOnFrame(vulkan_frame);
				vulkan_renderer.GetObjectPipeline().BindPipelineToFrame(vulkan_frame);
				vulkan_renderer.GetObjectPipeline().PrepareToBindDescriptorSets(vulkan_frame);
				auto object_entities = _ecs_registry.view<RenderableObject*>();
				for (auto e : object_entities)
				{
					RenderableObject& object = *ecs::Entity(e, _ecs_registry).Get<RenderableObject*>();
					object.RenderToFrame(vulkan_frame, vulkan_renderer.GetObjectPipeline());
				}
				vulkan_renderer.GetObjectPipeline().BindDescriptorSetsToFrame(vulkan_frame);

				/*
				vulkan_renderer.GetLightPipeline().BindPipelineToFrame(frame);
				vulkan_renderer.GetLightPipeline().SetUbo(ubo);
				vulkan_renderer.GetLightPipeline().PrepareToBindDescriptorSets(frame);
				auto light_entities = _ecs_registry.view<RenderableLight*>();
				for (auto e : light_entities)
				{
					RenderableLight& light = *ecs::Entity(e, _ecs_registry).Get<RenderableLight*>();
					light.RenderToFrame(frame, vulkan_renderer.GetLightPipeline());

					// TODO: we need to nest looping with the renderable objects to apply these lights to them
				}
				vulkan_renderer.GetLightPipeline().BindDescriptorSetsToFrame(frame);
				*/

				vulkan_renderer.EndRenderPassOnFrame(vulkan_frame);
				vulkan_frame.SubmitStagedCommandsToBuffer();

				_renderer.EndFrame();
				_renderer.DrawFrame();
			}
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