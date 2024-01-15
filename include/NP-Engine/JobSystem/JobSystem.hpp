//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_SYSTEM_HPP
#define NP_ENGINE_JOB_SYSTEM_HPP

#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "JobPriority.hpp"
#include "JobRecord.hpp"
#include "JobWorker.hpp"
#include "JobQueue.hpp"
#include "Job.hpp"

namespace np::jsys
{
	class JobSystem
	{
	private:
		friend class JobWorker;

		atm_bl _running;
		con::vector<JobWorker> _job_workers;
		mem::sptr<condition> _job_worker_sleep_condition;
		mem::TraitAllocator _allocator;
		mem::sptr<thr::ThreadPool> _thread_pool;
		mem::AccumulatingPool<Job> _job_pool;
		JobQueue _job_queue;

		JobRecord GetNextJob()
		{
			JobRecord next;
			for (siz i = 0; i < JobPrioritiesHighToLow.size(); i++)
			{
				next = _job_queue.Pop(JobPrioritiesHighToLow[i]);
				if (next.IsValid())
					break;
			}
			return next;
		}

		mem::sptr<condition> GetJobWorkerSleepCondition()
		{
			return _job_worker_sleep_condition;
		}

		mem::sptr<thr::Thread> CreateThread()
		{
			return _thread_pool->CreateObject();
		}

		siz GetThreadAffinity(siz worker_id)
		{
			// we add one to help prevent core 0 crowding -- assuming main thread is there
			return (worker_id + 1) % _thread_pool->GetObjectCount();
		}

	public:
		JobSystem(): _running(false), _thread_pool(nullptr)
		{
			SetDefaultJobWorkerCount();
		}

		~JobSystem()
		{
			Clear();
		}

		void Clear()
		{
			Stop();
			_job_workers.clear();
			_thread_pool.reset();
			_job_queue.Clear();
			_job_pool.Clear();
		}

		void SetDefaultJobWorkerCount()
		{
			// we want to be sure we use one less the number of cores available so our main thread is not crowded
			SetJobWorkerCount(thr::Thread::HardwareConcurrency() - 1);
		}

		void SetJobWorkerCount(siz count)
		{
			Stop();
			_job_workers.clear();
			_thread_pool = mem::create_sptr<thr::ThreadPool>(_allocator, count);

			for (siz i = 0; i < count; i++)
				_job_workers.emplace_back(i);
		}

		siz GetJobWokerCount() const
		{
			return _job_workers.size();
		}

		void Start()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			if (!_thread_pool)
				SetDefaultJobWorkerCount();

			_job_worker_sleep_condition = mem::create_sptr<condition>(_allocator);
			_running.store(true, mo_release);

			for (siz i = 0; i < _job_workers.size(); i++)
				_job_workers[i].StartWork(*this);
		}

		void Stop()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			if (IsRunning())
			{
				for (siz i = 0; i < _job_workers.size(); i++)
					_job_workers[i].StopWork();

				_running.store(false, mo_release);
				_job_worker_sleep_condition.reset();
			}
		}

		bl IsRunning() const
		{
			return _running.load(mo_acquire);
		}

		mem::sptr<Job> CreateJob()
		{
			return _job_pool.CreateObject();
		}

		void SubmitJob(JobPriority priority, mem::sptr<Job> job)
		{
			_job_queue.Push(priority, job);

			for (siz i = 0; i < _job_workers.size(); i++)
				_job_workers[i].WakeUp();

			if (_job_worker_sleep_condition)
				_job_worker_sleep_condition->notify_one();
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_SYSTEM_HPP */
