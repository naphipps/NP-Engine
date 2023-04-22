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

#include "Layer.hpp"
#include "Popup.hpp"

#ifndef NP_ENGINE_RENDERING_LOOP_DURATION
	#define NP_ENGINE_RENDERING_LOOP_DURATION 0 // milliseconds
#endif

namespace np::app
{
	class GraphicsLayer : public Layer //TODO: I'm tempted to refactor all "Graphics" things into "GPU" with namespace "gpu" since we use the gpu for so much more than graphics
	{
	protected:
		Mutex _scenes_mutex;
		con::vector<mem::wptr<gfx::Scene>> _scenes;
		siz _job_worker_index;
		mem::sptr<jsys::Job> _rendering_job;
		atm_bl _keep_rendering;

		static void RenderingCallback(void* caller, mem::Delegate& d)
		{
			((GraphicsLayer*)caller)->RenderingProcedure(d);
		}

		void RenderingProcedure(mem::Delegate& d)
		{
			NP_ENGINE_PROFILE_FUNCTION();

			_keep_rendering.store(true, mo_release);
			tim::SteadyTimestamp next = tim::SteadyClock::now();
			tim::SteadyTimestamp prev = next;
			const tim::DblMilliseconds min_duration(NP_ENGINE_RENDERING_LOOP_DURATION);

			while (_keep_rendering.load(mo_acquire))
			{
				NP_ENGINE_PROFILE_SCOPE("rendering loop");

				/*
				{
					Lock l(_scenes_mutex);
					for (auto it = _scenes.begin(); it != _scenes.end(); it++)
					{
						mem::sptr<gfx::Scene> scene = it->get_sptr();
						if (scene)
							scene->Render();
					}
				}
				//*/
				
				for (next = tim::SteadyClock::now(); next - prev < min_duration; next = tim::SteadyClock::now())
					thr::ThisThread::yield();
				prev = next;
			}
		}

	public:
		GraphicsLayer(mem::sptr<srvc::Services> services): 
			Layer(services), 
			_job_worker_index(0),
			_rendering_job(nullptr), 
			_keep_rendering(false)
		{}

		virtual ~GraphicsLayer()
		{
			StopRenderingJob();
		}

		virtual void Cleanup() override
		{
			Lock l(_scenes_mutex);
			for (siz i = _scenes.size() - 1; i < _scenes.size(); i--)
				if (_scenes[i].is_expired())
					_scenes.erase(_scenes.begin() + i);
		}

		void RegisterScene(mem::sptr<gfx::Scene> scene)
		{
			Lock l(_scenes_mutex);
			_scenes.emplace_back(scene);
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

#endif /* NP_ENGINE_GRAPHICS_LAYER_HPP */
