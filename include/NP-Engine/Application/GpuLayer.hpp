//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/19/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_GPU_LAYER_HPP
#define NP_ENGINE_GPU_LAYER_HPP

#include <utility>

#include "NP-Engine/GPU/GPU.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Services/Services.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "NP-Engine/Window/Interface/WindowEvents.hpp"

#include "Layer.hpp"
#include "Popup.hpp"

namespace np::app
{
	class GpuLayer : public Layer
	{
	protected:
		mutexed_wrapper<con::vector<mem::wptr<gpu::Scene>>> _scenes;
		siz _job_worker_index;
		mem::sptr<jsys::Job> _rendering_job;
		atm_bl _keep_rendering;

		static void RenderingCallback(void* caller, mem::Delegate& d)
		{
			((GpuLayer*)caller)->RenderingProcedure(d);
		}

		void RenderingProcedure(mem::Delegate& d)
		{
			NP_ENGINE_PROFILE_FUNCTION();

			_keep_rendering.store(true, mo_release);
			tim::SteadyTimestamp next = tim::SteadyClock::now();
			tim::SteadyTimestamp prev = next;
			const tim::DblMilliseconds min_duration(2); //1.66666667 -> 600fps, 2 -> 500fps

			tim::SteadyTimestamp frame_next = next;
			tim::SteadyTimestamp frame_prev = next;
			const tim::DblMilliseconds one_second(1000);
			siz frame_count = 0;
			siz fps = 0;

			while (_keep_rendering.load(mo_acquire))
			{
				NP_ENGINE_PROFILE_SCOPE("rendering loop");
				{
					auto scenes = _scenes.get_access();
					for (auto it = scenes->begin(); it != scenes->end(); it++)
					{
						mem::sptr<gpu::Scene> scene = it->get_sptr();
						if (scene)
							scene->Render();
					}
				}
				
				frame_count++;
				frame_next = tim::SteadyClock::now();
				if (frame_next - frame_prev >= one_second)
				{
					fps = frame_count;
					frame_count = 0;
					frame_prev = frame_next;

					//NP_ENGINE_LOG_INFO("FPS: " + to_str(fps));

					if (false)
					{
						auto scenes = _scenes.get_access();
						for (auto it = scenes->begin(); it != scenes->end(); it++)
						{
							mem::sptr<gpu::Scene> scene = it->get_sptr();
							if (scene)
								scene->GetRenderTarget()->GetWindow()->SetTitle("FPS: " + to_str(fps)); //245-250 so far
						}
					}
				}

				for (next = tim::SteadyClock::now(); next - prev < min_duration; next = tim::SteadyClock::now())
					thr::ThisThread::yield();
				prev = next;

				thr::ThisThread::yield();
			}
		}

	public:
		GpuLayer(mem::sptr<srvc::Services> services): 
			Layer(services), 
			_job_worker_index(0),
			_rendering_job(nullptr), 
			_keep_rendering(false)
		{}

		virtual ~GpuLayer()
		{
			StopRenderingJob();
		}

		virtual void Cleanup() override
		{
			auto scenes = _scenes.try_get_access_for(tim::DblMilliseconds(0.3));
			if (scenes)
				for (siz i = scenes->size() - 1; i < scenes->size(); i--)
					if ((*scenes)[i].is_expired())
						scenes->erase(scenes->begin() + i);
		}

		void Register(mem::sptr<gpu::Scene> scene)
		{
			bl found = false;
			auto scenes = _scenes.get_access();
			for (auto it = scenes->begin(); !found && it != scenes->end(); it++)
				found = it->get_sptr() == scene;

			if (!found)
				scenes->emplace_back(scene);
		}

		void Unregister(mem::sptr<gpu::Scene> scene)
		{
			auto scenes = _scenes.get_access();
			for (auto it = scenes->begin(); it != scenes->end(); it++)
			{
				if (it->get_sptr() == scene)
				{
					scenes->erase(it);
					break;
				}
			}
		}

		void SetJobWorkerIndex(siz index)
		{
			_job_worker_index = index;
		}

		void SubmitRenderingJob()
		{
			if (!_rendering_job)
			{
				_rendering_job = _services->GetJobSystem().CreateJob();
				_rendering_job->GetDelegate().SetCallback(this, RenderingCallback);
				_rendering_job->SetCanBeStolen(false);
				_services->GetJobSystem().GetJobWorkers()[_job_worker_index].SubmitImmediateJob(_rendering_job);
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

				_rendering_job.reset();
			}
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_GPU_LAYER_HPP */