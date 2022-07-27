//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_SYSTEM_IMPL_HPP
#define NP_ENGINE_JOB_SYSTEM_IMPL_HPP

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Math/Math.hpp"

#include "JobWorker.hpp"
#include "JobWorkerToken.hpp"
#include "JobPool.hpp"

namespace np::js
{
	class JobSystem
	{
	private:
		container::array<JobWorker, concurrency::ThreadPool::MAX_THREAD_COUNT> _job_workers;
		siz _job_worker_count;
		concurrency::ThreadPool _thread_pool;
		JobPool _job_pool;
		bl _running;

	public:
		JobSystem(): _job_pool(2000), _running(false)
		{
			// we want to be sure we use one less the number of cores available so our main thread is not crowded
			_job_worker_count = math::min(_thread_pool.ObjectCount() - 1, _job_workers.size());

			for (ui32 i = 0; i < _job_worker_count; i++)
			{
				_job_workers[i]._job_pool = &_job_pool;

				for (ui32 j = 0; j < _job_worker_count; j++)
				{
					_job_workers[i].AddCoworker(&_job_workers[j]);
				}
			}
		}

		~JobSystem()
		{
			if (IsRunning())
			{
				Stop();
			}
		}

		void Start()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			_running = true;

			for (siz i = 0; i < _job_worker_count; i++)
			{
				_job_workers[i].StartWork(_thread_pool, (i + 1) % _thread_pool.ObjectCount());
			}
		}

		void Stop()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			if (IsRunning())
			{
				// stop workers
				for (ui64 i = 0; i < _job_worker_count; i++)
				{
					_job_workers[i].StopWork();
				}

				// stop threads
				_thread_pool.Clear();
				_running = false;
			}
		}

		bl IsRunning()
		{
			return _running;
		}

		ui64 GetJobWorkerCount()
		{
			return _job_worker_count;
		}

		JobWorkerToken GetJobWorker(ui64 index)
		{
			return JobWorkerToken(&_job_workers[index]);
		}
	};
} // namespace np::js

#endif /* NP_ENGINE_JOB_SYSTEM_IMPL_HPP */
