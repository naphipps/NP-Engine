//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GRAPHICS_LAYER_HPP
#define NP_ENGINE_GRAPHICS_LAYER_HPP

#include <utility>

#include "NP-Engine/Graphics/Graphics.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "NP-Engine/Window/Interface/WindowEvents.hpp"

#include "NP-Engine/Graphics/RHI/Vulkan/VulkanGraphics.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX || NP_ENGINE_PLATFORM_IS_WINDOWS
	#include "NP-Engine/Graphics/RHI/OpenGL/OpenGLGraphics.hpp"
#endif

#include "Layer.hpp"
#include "Popup.hpp"

// TODO: looks like our big-picture is going to expand to support a Task Graph Architecture of some sort...
// TODO: ^ Halcyon's RenderGraph idea is pretty nice, but it is inspired by Frostbite's FrameGraph [O'Donnell 2017]

#ifndef NP_ENGINE_RENDERING_LOOP_DURATION
	#define NP_ENGINE_RENDERING_LOOP_DURATION 0 // milliseconds
#endif

namespace np::app
{
	class GraphicsLayer : public Layer
	{
	protected:
		con::vector<gfx::Renderer*> _renderers;
		con::vector<gfx::Scene*> _scenes;

		siz _job_worker_index;
		jsys::Job* _rendering_job;
		atm_bl _keep_rendering;

		void AdjustForWindowClosingProcedure(mem::Delegate& d)
		{
			win::Window* window = d.GetData<win::Window*>();

			for (auto it = _scenes.begin(); it != _scenes.end(); it++)
				if ((*it)->GetRenderer().IsAttachedToWindow(*window))
				{
					mem::Destroy<gfx::Scene>(_services.GetAllocator(), *it);
					_scenes.erase(it);
					break;
				}

			for (auto it = _renderers.begin(); it != _renderers.end(); it++)
				if ((*it)->IsAttachedToWindow(*window))
				{
					(*it)->DetachFromWindow(*window);
					mem::Destroy<gfx::Renderer>(_services.GetAllocator(), *it);
					_renderers.erase(it);
					break;
				}
		}

		virtual void AdjustForWindowClosing(evnt::Event& e)
		{
			win::WindowClosingEvent::DataType& closing_data = e.GetData<win::WindowClosingEvent::DataType>();

			jsys::Job* adjust_job = _services.GetJobSystem().CreateJob();
			adjust_job->GetDelegate().Connect<GraphicsLayer, &GraphicsLayer::AdjustForWindowClosingProcedure>(this);

			closing_data.job->AddDependency(*adjust_job);
			_services.GetJobSystem().SubmitJob(jsys::JobPriority::Higher, adjust_job);
		}

		virtual void HandleEvent(evnt::Event& e) override
		{
			switch (e.GetType())
			{
			case evnt::EventType::WindowClosing:
				AdjustForWindowClosing(e);
				break;
			default:
				break;
			}
		}

		void ChooseGraphicsDetailType()
		{
			gfx::__detail::RegisteredRhiType.store(gfx::RhiType::Vulkan);

			// TODO: we need to redo how we choose our rhi... look at Renderer.IsValid methods or something similar??
			// TODO: we might ought to interact with a config file and the user (popups) to determine??
		}





		class FramePacket
		{
		private:
			Mutex _mutex;
			ecs::Registry _registry;
			con::uset<ecs::Entity> _entities_to_add;
			con::uset<ecs::Entity> _entities_to_remove;

		public:

			void RemoveEntities()
			{
				Lock lock(_mutex);
			}

			void AddEntities()
			{
				Lock lock(_mutex);
			}

			void SetEntityToRemove(ecs::Entity e)
			{
				Lock lock(_mutex);
			}

			void SetEntityToAdd(ecs::Entity e)
			{
				Lock lock(_mutex);
			}
		};




		atm<FramePacket*> _staged_for_producing;
		atm<FramePacket*> _producing;
		atm<FramePacket*> _staged_for_consuming;
		atm<FramePacket*> _consuming;


		FramePacket* GetNextFramePacket()
		{
			FramePacket* packet = nullptr;

			packet = _staged_for_consuming.load(mo_acquire);
			_staged_for_consuming.store(nullptr);

			return packet;
		}

		void RenderingProcedure(mem::Delegate& d)
		{
			NP_ENGINE_PROFILE_FUNCTION();

			_keep_rendering.store(true, mo_release);
			tim::SteadyTimestamp next = tim::SteadyClock::now();
			tim::SteadyTimestamp prev = next;
			const tim::DblMilliseconds min_duration(NP_ENGINE_RENDERING_LOOP_DURATION);

			

			
			FramePacket* prev_packet;
			FramePacket* packet = nullptr;

			while (_keep_rendering.load(mo_acquire))
			{
				//NP_ENGINE_PROFILE_SCOPE("rendering loop");

				prev_packet = packet;
				packet = GetNextFramePacket();

				if (packet)
				{
					NP_ENGINE_PROFILE_SCOPE("frame packet:" + to_str((siz)packet));

					if (packet == prev_packet)
					{
						NP_ENGINE_PROFILE_SCOPE("consider velocity");
					}
					else
					{
						NP_ENGINE_PROFILE_SCOPE("ignore velocity");
					}
				}
				else
				{
					NP_ENGINE_PROFILE_SCOPE("no frame packet");
				}

				for (next = tim::SteadyClock::now(); next - prev < min_duration; next = tim::SteadyClock::now())
					thr::ThisThread::yield();
				prev = next;
			}
		}

	public:
		GraphicsLayer(srvc::Services& services): Layer(services), _job_worker_index(0), _rendering_job(nullptr), _keep_rendering(false)
		{
			ChooseGraphicsDetailType();
		}

		virtual ~GraphicsLayer()
		{
			StopRenderingJob();

			for (auto it = _scenes.begin(); it != _scenes.end(); it++)
				mem::Destroy<gfx::Scene>(_services.GetAllocator(), *it);

			for (auto it = _renderers.begin(); it != _renderers.end(); it++)
				mem::Destroy<gfx::Renderer>(_services.GetAllocator(), *it);
		}

		gfx::Renderer* CreateRenderer(win::Window& window)
		{
			gfx::Renderer* renderer = gfx::Renderer::Create(_services);
			renderer->AttachToWindow(window);
			return _renderers.emplace_back(renderer);
		}

		gfx::Scene* CreateScene(gfx::Renderer& renderer)
		{
			gfx::Scene* scene = gfx::Scene::Create(_services, renderer);
			return _scenes.emplace_back(scene);
		}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return (evnt::EventCategory)((ui64)evnt::EventCategory::Graphics | (ui64)evnt::EventCategory::Window);
		}

		void SetJobWorkerIndex(siz index)
		{
			_job_worker_index = index;
		}

		void SubmitRenderingJob()
		{
			if (!_rendering_job)
			{
				_rendering_job = _services.GetJobSystem().CreateJob();
				_rendering_job->GetDelegate().Connect<GraphicsLayer, &GraphicsLayer::RenderingProcedure>(this);
				_rendering_job->SetCanBeStolen(false);
				_services.GetJobSystem().GetJobWorkers()[_job_worker_index].SubmitImmediateJob(_rendering_job);
			}
			else
			{
				NP_ENGINE_ASSERT(false, "call StopRenderingJob before calling SubmitRenderingJob again");
			}
		}

		void StopRenderingJob()
		{
			if (_rendering_job)
			{
				while (_rendering_job->IsEnabled() && !_rendering_job->IsComplete())
					_keep_rendering.store(false, mo_release);

				_rendering_job = nullptr;
			}
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_GRAPHICS_LAYER_HPP */
