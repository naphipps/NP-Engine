//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/15/22
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/JobSystem/JobWorker.hpp"
#include "NP-Engine/JobSystem/JobSystemImpl.hpp" //get the defined JobSystem since we forward-declared it in JobWorker

namespace np::jsys
{
	JobRecord JobWorker::GetNextJob()
	{
		NP_ENGINE_PROFILE_FUNCTION();

		JobRecord next_job;
		for (siz i = 0; i < JobPrioritiesHighToLow.size() && !next_job.IsValid(); i++)
		{
			_job_system->GetQueueForPriority(JobPrioritiesHighToLow[i]).try_dequeue(next_job);
			if (next_job.IsValid())
			{
				Job* job = next_job.GetJob();
				if (job->IsEnabled())
				{
					if (!job->CanExecute())
					{
						// TODO: pretty sure using NormalizePriority function is faster, so profile this
						_job_system->SubmitJob(job->GetAttractedPriority(next_job.GetPriority()), job);
						next_job.Invalidate();
					}
				}
				else
				{
					NP_ENGINE_ASSERT(false, "next_job must be enabled when valid at all times - probable memory leak");
				}
			}
		}
		return next_job;
	}

	void JobWorker::WorkerThreadProcedure()
	{
		NP_ENGINE_PROFILE_SCOPE("WorkerThreadProcedure: " + to_str((ui64)this));

		Job* immediate = nullptr;
		JobRecord record;
		Job* j = nullptr;
		Job* stolen = nullptr;

		while (_keep_working.load(mo_acquire))
		{
			immediate = GetImmediateJob();
			if (immediate)
			{
				NP_ENGINE_PROFILE_SCOPE("executing immediate Job");
				immediate->Execute();
				if (immediate->IsComplete())
					_job_system->DestroyJob(immediate);
				else
					SubmitImmediateJob(immediate);
			}
			else if (_keep_working.load(mo_acquire))
			{
				record = GetNextJob();
				if (record.IsValid())
				{
					NP_ENGINE_PROFILE_SCOPE("executing next Job");
					j = record.GetJob();
					j->Execute();
					if (j->IsComplete())
						_job_system->DestroyJob(j);
					else
						_job_system->SubmitJob(j->GetAttractedPriority(record.GetPriority()), j);
				}
				else if (_keep_working.load(mo_acquire))
				{
					// we did not find next job so we steal from coworker
					stolen = GetStolenJob();
					if (stolen)
					{
						NP_ENGINE_PROFILE_SCOPE("executing stolen Job");
						stolen->Execute();
						if (stolen->IsComplete())
							_job_system->DestroyJob(stolen);
						else
							SubmitImmediateJob(stolen);
					}
					else if (_keep_working.load(mo_acquire))
					{
						NP_ENGINE_PROFILE_SCOPE("sleeping");

						// we did not steal a good job thus we want to steal from someone else
						_coworker_index = (_coworker_index + 1) % _coworkers.size();

						// we yield/sleep just in case all jobs are done
						thr::ThisThread::yield();
						thr::ThisThread::sleep_for(_bad_steal_sleep_duration);
					}
				}
			}
		}

		_work_procedure_complete.store(true, mo_release);
	}
} // namespace np::jsys