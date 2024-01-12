//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_WORKER_HPP
#define NP_ENGINE_JOB_WORKER_HPP

#ifndef NP_ENGINE_JOB_WORKER_SLEEP_DURATION
	#define NP_ENGINE_JOB_WORKER_SLEEP_DURATION 1 // milliseconds
#endif

#ifndef NP_ENGINE_JOB_WORKER_DEEP_SLEEP_DURATION
	#define NP_ENGINE_JOB_WORKER_DEEP_SLEEP_DURATION 3 // milliseconds
#endif

#include <utility>
#include <optional>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Random/Random.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/String/String.hpp"

#include "JobRecord.hpp"
#include "JobQueue.hpp"

namespace np::jsys
{
	class JobWorker
	{
	private:
		friend class JobSystem;

		siz _id;
		atm_bl _keep_working;
		mem::sptr<thr::Thread> _thread;
		JobQueue& _job_queue;
		atm<::std::optional<ui32>> _deep_sleep_threshold;

		/*
			returns a valid && CanExecute() job, or invalid job
		*/
		JobRecord GetNextJob()
		{
			JobRecord next_job;
			for (siz i = 0; i < JobPrioritiesHighToLow.size() && !next_job.IsValid(); i++)
			{
				next_job = _job_queue.Pop(JobPrioritiesHighToLow[i]);
				if (next_job.IsValid())
				{
					if (next_job.job->IsEnabled())
					{
						if (!next_job.job->CanExecute())
						{
							_job_queue.Push(NormalizePriority(next_job.priority), next_job.job);
							next_job.Invalidate(); // done with record
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

		void WorkerThreadProcedure()
		{
			NP_ENGINE_PROFILE_SCOPE("WorkerThreadProcedure: " + to_str((ui64)this));

			ui32 sleep_count = 0;
			bl success = false;
			while (_keep_working.load(mo_acquire))
			{
				success = TryPriorityBasedJob();

				if (_keep_working.load(mo_acquire))
				{
					if (success)
					{
						success = false;
						sleep_count = 0;
					}
					else
					{
						::std::optional<ui32> deep_sleep_threshold = _deep_sleep_threshold.load(mo_acquire);
						if (!deep_sleep_threshold.has_value())
							deep_sleep_threshold = thr::Thread::HardwareConcurrency() * 3; //arbitrary

						if (sleep_count < deep_sleep_threshold.value())
						{
							const tim::DblMilliseconds duration(NP_ENGINE_JOB_WORKER_SLEEP_DURATION);
							const tim::SteadyTimestamp start = tim::SteadyClock::now();
							while (tim::SteadyClock::now() - start < duration)
								thr::ThisThread::yield();

							sleep_count++;
						}
						else
						{
							thr::ThisThread::sleep_for(tim::DblMilliseconds(NP_ENGINE_JOB_WORKER_DEEP_SLEEP_DURATION));
						}
					}
				}
			}
		}

		bl TryPriorityBasedJob()
		{
			bl success = !_keep_working.load(mo_acquire);

			if (!success)
			{
				JobRecord record = GetNextJob();
				if (record.IsValid())
				{
					NP_ENGINE_PROFILE_SCOPE("executing next Job");
					success = true;
					(*record.job)(_id);

					if (!record.job->IsComplete())
						_job_queue.Push(NormalizePriority(record.priority), record.job);

					record.Invalidate(); // done with record
				}
			}

			return success;
		}

	public:
		JobWorker(siz id, JobQueue& job_queue): _id(id), _keep_working(false), _thread(nullptr), _job_queue(job_queue)
		{}

		JobWorker(JobWorker&& other) noexcept:
			_id(::std::move(other._id)),
			_keep_working(::std::move(other._keep_working.load(mo_acquire))),
			_thread(::std::move(other._thread)),
			_job_queue(other._job_queue)
		{}

		virtual ~JobWorker()
		{
			StopWork();
		}

		void SetDeepSleepThreshold(ui32 threshold)
		{
			_deep_sleep_threshold.store(threshold, mo_release);
		}

		void ResetDeepSleepThreshold()
		{
			_deep_sleep_threshold.store(::std::optional<ui32>(), mo_release);
		}

		void DisableDeepSleep()
		{
			_deep_sleep_threshold.store(UI32_MAX, mo_release);
		}

		void StartWork(thr::ThreadPool& thread_pool, siz thread_affinity)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			_keep_working.store(true, mo_release);
			_thread = thread_pool.CreateObject();
			_thread->Run(&JobWorker::WorkerThreadProcedure, this);
			_thread->SetAffinity(thread_affinity);
		}

		void StopWork()
		{
			NP_ENGINE_PROFILE_FUNCTION();
			_keep_working.store(false, mo_release);
			_thread.reset();
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_WORKER_HPP */
