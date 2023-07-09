//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_INTERFACE_SCENE_HPP
#define NP_ENGINE_GPU_INTERFACE_SCENE_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Services/Services.hpp"

#include "DetailType.hpp"
#include "RenderPipeline.hpp"
#include "Camera.hpp"

namespace np::gpu
{
	class Scene
	{
	public:
		struct Properties
		{
			mem::sptr<RenderPipeline> renderPipeline;
			Camera camera;
		};

	protected:
		Properties _properties;
		mutexed_wrapper<con::umap<uid::Uid, mem::sptr<VisibleObject>>> _visibles;
		mem::Delegate _on_render_delegate;

		/*
		struct RenderScenePayload
		{
			mem::sptr<CommandStaging> staging;
			Camera camera;
		};

		struct RenderVisiblePayload
		{
			const RenderScenePayload* scenePayload;
			VisibleObject visible;
			uid::Uid id;
		};
		//*/

		static void BeginRenderSceneCallback(void* caller, mem::Delegate& d)
		{
			((Scene*)caller)->BeginRenderSceneProcedure(d);
		}

		virtual void BeginRenderSceneProcedure(mem::Delegate& d)
		{
			// RenderScenePayload* payload = d.GetData<RenderScenePayload*>();
			mem::sptr<RenderContext> context = GetRenderContext();
			mem::sptr<RenderPipeline> pipeline = GetRenderPipeline();

			// payload->staging = context->ProduceCommandStaging();
			// context->Begin(*payload->staging);
			// pipeline->BeginRenderPass(*payload->staging);
		}

		static void EndRenderSceneCallback(void* caller, mem::Delegate& d)
		{
			((Scene*)caller)->EndRenderSceneProcedure(d);
		}

		virtual void EndRenderSceneProcedure(mem::Delegate& d)
		{
			// RenderScenePayload* payload = d.GetData<RenderScenePayload*>();
			// RenderContext& context = GetRenderContext();
			// RenderPipeline& pipeline = GetRenderPipeline();

			// pipeline.EndRenderPass(*payload->staging);
			// payload->staging->DigestCommands();
			//.End(*payload->staging);
			// context.ConsumeCommandStaging(*payload->staging);

			// mem::Destroy<RenderScenePayload>(GetServices()->GetAllocator(), payload);
		}

		static void RenderVisibleCallback(void* caller, mem::Delegate& d)
		{
			((Scene*)caller)->RenderVisibleProcedure(d);
		}

		virtual void RenderVisibleProcedure(mem::Delegate& d)
		{
			// RenderVisiblePayload* payload = d.GetData<RenderVisiblePayload*>();
			mem::sptr<Resource> resource; // = GetRenderDevice().GetResource(payload->id);

			// if (resource && payload->scenePayload->camera.Contains(payload->visible))
			// StageResource(resource, *payload);

			// TODO: ^ figure out how to get animation data ?

			// mem::Destroy<RenderVisiblePayload>(GetServices()->GetAllocator(), payload);
		}

		// virtual void StageResource(mem::sptr<Resource> resource, RenderVisiblePayload& payload) = 0;

		Scene(Properties& properties): _properties(properties) {}

	public:
		static mem::sptr<Scene> Create(Properties properties);

		virtual ~Scene()
		{
			auto visibles = _visibles.get_access();
			for (auto it = visibles->begin(); it != visibles->end(); it++)
				UnregisterResource(it->first);
		}

		virtual DetailType GetDetailType() const
		{
			return GetRenderPipeline()->GetDetailType();
		}

		virtual mem::sptr<srvc::Services> GetServices() const
		{
			return GetRenderPipeline()->GetServices();
		}

		virtual void SetCamera(Camera& camera)
		{
			_properties.camera = camera;
		}

		virtual Camera GetCamera() const
		{
			return _properties.camera;
		}

		virtual mem::sptr<RenderPipeline> GetRenderPipeline() const
		{
			return _properties.renderPipeline;
		}

		virtual mem::sptr<RenderDevice> GetRenderDevice() const
		{
			return GetRenderContext()->GetRenderDevice();
		}

		virtual mem::sptr<RenderContext> GetRenderContext() const
		{
			return GetRenderPipeline()->GetProperties().framebuffers->GetRenderPass()->GetRenderContext();
		}

		virtual mem::sptr<RenderTarget> GetRenderTarget() const
		{
			return GetRenderDevice()->GetRenderTarget();
		}

		Properties GetProperties() const
		{
			return _properties;
		}

		mem::Delegate& GetOnRenderDelegate()
		{
			return _on_render_delegate;
		}

		const mem::Delegate& GetOnRenderDelegate() const
		{
			return _on_render_delegate;
		}

		virtual mem::sptr<Resource> CreateResource(mem::sptr<Model> model) = 0; // TODO: for image and light too

		virtual void Register(uid::Uid id, mem::sptr<VisibleObject> visible, mem::sptr<Model> model)
		{
			Register(id, visible);
			Register(id, model);
		}

		virtual void Register(uid::Uid id, mem::sptr<VisibleObject> visible)
		{
			(*_visibles.get_access())[id] = visible;
		}

		virtual void Register(uid::Uid id, mem::sptr<Model> model) // TODO: for image and light too
		{
			GetRenderDevice()->Register(id, CreateResource(model));
		}

		virtual void Unregister(uid::Uid id)
		{
			UnregisterVisible(id);
			UnregisterResource(id);
		}

		virtual void UnregisterVisible(uid::Uid id)
		{
			_visibles.get_access()->erase(id);
		}

		virtual void UnregisterResource(uid::Uid id)
		{
			GetRenderDevice()->Unregister(id);
		}

		virtual mem::sptr<Resource> GetResource(uid::Uid id)
		{
			return GetRenderDevice()->GetResource(id);
		}

		virtual bl HasResource(uid::Uid id)
		{
			return GetRenderDevice()->HasResource(id);
		}

		virtual void CleanupVisibles()
		{
			uid::UidSystem& uid_system = GetServices()->GetUidSystem();
			auto visibles = _visibles.get_access();
			for (auto it = visibles->begin(); it != visibles->end();)
			{
				if (!uid_system.Has(it->first))
				{
					GetRenderDevice()->Unregister(it->first);
					it = visibles->erase(it);
				}
				else
				{
					it++;
				}
			}
		}

		virtual void BeginRenderPass(mem::sptr<CommandStaging> staging) = 0;

		virtual void EndRenderPass(mem::sptr<CommandStaging> staging) = 0;

		virtual void Render() = 0;

		virtual void Render2()
		{
			/*
			mem::sptr<srvc::Services> services = GetServices();
			jsys::JobSystem& job_system = services->GetJobSystem();
			mem::Allocator& allocator = services->GetAllocator();

			mem::sptr<RenderScenePayload> render_scene_payload = mem::create_sptr<RenderScenePayload>(allocator, nullptr,
			GetCamera());

			mem::sptr<jsys::Job> begin_render_scene_job = job_system.CreateJob();
			begin_render_scene_job->GetDelegate().ConstructData<mem::sptr<RenderScenePayload>>(render_scene_payload);
			begin_render_scene_job->GetDelegate().SetCallback(this, BeginRenderSceneCallback);

			mem::sptr<jsys::Job> end_render_scene_job = job_system.CreateJob();
			end_render_scene_job->GetDelegate().ConstructData<mem::sptr<RenderScenePayload>>(render_scene_payload);
			end_render_scene_job->GetDelegate().SetCallback(this, EndRenderSceneCallback);
			jsys::Job::AddDependency(end_render_scene_job, begin_render_scene_job);
			//render_scene_payload->endJob = end_render_scene_job;

			mem::sptr<jsys::Job> render_visible_job = nullptr;
			mem::sptr<RenderVisiblePayload> render_visible_payload = nullptr;
			//*/

			// TODO: how do we allow the following jobs to organize/digest resources by type then come together before the end
			// job??

			/*
				TODO:
					- On scene render, decide if the end job is going to be a deferred render frame or forward.
						Then make checking jobs create commands based on that. We can collect those commands in a
						mpmc queue or something on scene payload

					- Extract AABB from model to determine size and where the camera should be
			*/

			/*
			{
				Lock l(_visibles_mutex);
				for (auto it = _visibles.begin(); it != _visibles.end(); it++)
				{
					//render_visible_payload = mem::create_sptr<RenderVisiblePayload>(allocator, render_scene_payload,
			*it->first, it->second); render_visible_job = job_system.CreateJob();
					//render_visible_job->GetDelegate().ConstructData<RenderVisiblePayload*>(render_visible_payload); #error
			//TODO: invalid construction, type is sptr render_visible_job->GetDelegate().SetCallback(this,
			RenderVisibleCallback);

					//render_visible_job->AddDependency(*begin_render_scene_job);
					//end_render_scene_job->AddDependency(*render_visible_job);

					job_system.SubmitJob(jsys::JobPriority::Normal, render_visible_job);
				}
			}

			job_system.SubmitJob(jsys::JobPriority::Normal, end_render_scene_job);
			job_system.SubmitJob(jsys::JobPriority::Higher, begin_render_scene_job);
			//*/
		}
	};
} // namespace np::gpu

#endif /* NP_ENGINE_GPU_INTERFACE_SCENE_HPP */