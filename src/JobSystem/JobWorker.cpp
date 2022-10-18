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
						_job_system->SubmitJob(NormalizePriority(next_job.GetPriority()), job);
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

	bl JobWorker::TryImmediateJob()
	{
		bl success = !_keep_working.load(mo_acquire);

		if (!success)
		{
			Job* immediate = GetImmediateJob();
			if (immediate)
			{
				NP_ENGINE_PROFILE_SCOPE("executing immediate Job");
				success = true;
				immediate->Execute();
				if (immediate->IsComplete())
					_job_system->DestroyJob(immediate);
				else
					SubmitImmediateJob(immediate);
			}
		}

		return success;
	}

	bl JobWorker::TryPriorityBasedJob()
	{
		bl success = !_keep_working.load(mo_acquire);

		if (!success)
		{
			JobRecord record = GetNextJob();
			if (record.IsValid())
			{
				NP_ENGINE_PROFILE_SCOPE("executing next Job");
				success = true;
				Job* job = record.GetJob();
				job->Execute();
				if (job->IsComplete())
					_job_system->DestroyJob(job);
				else
					_job_system->SubmitJob(NormalizePriority(record.GetPriority()), job);
			}
		}

		return success;
	}

	bl JobWorker::TryStealingJob()
	{
		bl success = !_keep_working.load(mo_acquire);

		if (!success)
		{
			Job* stolen = GetStolenJob();
			if (stolen)
			{
				NP_ENGINE_PROFILE_SCOPE("executing stolen Job");
				success = true;
				stolen->Execute();
				if (stolen->IsComplete())
					_job_system->DestroyJob(stolen);
				else
					SubmitImmediateJob(stolen);
			}
			else
			{
				// we did not steal a good job thus we want to steal from someone else
				_coworker_index = (_coworker_index + 1) % _coworkers.size();
			}
		}

		return success;
	}

	void JobWorker::WorkerThreadProcedure()
	{
		NP_ENGINE_PROFILE_SCOPE("WorkerThreadProcedure: " + to_str((ui64)this));

		ui32 deep_sleep_threshold = _coworkers.size() * 3;
		ui32 sleep_count = 0;
		FetchOrderArray fetch_order;
		bl success = false;
		while (_keep_working.load(mo_acquire))
		{
			fetch_order = GetFetchOrder();
			for (const Fetch& fetch : fetch_order)
			{
				switch (fetch)
				{
				case Fetch::Immediate:
					success = TryImmediateJob();
					break;
				case Fetch::PriorityBased:
					success = TryPriorityBasedJob();
					break;
				case Fetch::Steal:
					success = TryStealingJob();
					break;
				case Fetch::None:
				default:
					continue;
					break;
				}

				if (success)
					break;
			}

			if (_keep_working.load(mo_acquire))
			{
				if (success)
				{
					success = false;
					sleep_count = 0;
				}
				else
				{
					if (sleep_count < deep_sleep_threshold)
					{
						const tim::DblMilliseconds duration((dbl)NP_ENGINE_APPLICATION_LOOP_DURATION / 2.0);
						const tim::SteadyTimestamp start = tim::SteadyClock::now();

						do
							thr::ThisThread::yield();
						while (tim::SteadyClock::now() - start < duration);

						sleep_count++;
					}
					else
					{
						thr::ThisThread::sleep_for(tim::DblMilliseconds(NP_ENGINE_APPLICATION_LOOP_DURATION));
					}
				}
			}
		}

		_work_procedure_complete.store(true, mo_release);
	}
} // namespace np::jsys