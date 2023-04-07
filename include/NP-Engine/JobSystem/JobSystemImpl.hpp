//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_SYSTEM_IMPL_HPP
#define NP_ENGINE_JOB_SYSTEM_IMPL_HPP

#include <utility>

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "JobWorker.hpp"
#include "JobPool.hpp"

namespace np::jsys
{
	class JobSystem
	{
	private:
		atm_bl _running;
		con::vector<JobWorker> _job_workers;
		mem::TraitAllocator _thread_pool_allocator;
		mem::sptr<thr::ThreadPool> _thread_pool;
		JobPool _job_pool;

		// TODO: use tokens with mpmc_queue
		con::mpmc_queue<JobRecord> _highest_job_queue;
		con::mpmc_queue<JobRecord> _higher_job_queue;
		con::mpmc_queue<JobRecord> _normal_job_queue;
		con::mpmc_queue<JobRecord> _lower_job_queue;
		con::mpmc_queue<JobRecord> _lowest_job_queue;

	public:
		JobSystem(): _running(false), _thread_pool(nullptr)
		{
			SetDefaultJobWorkerCount();
		}

		~JobSystem()
		{
			Dispose();
		}

		void Dispose()
		{
			Stop();
			
			JobRecord record{};
			for (auto priority_it = JobPrioritiesHighToLow.begin(); priority_it != JobPrioritiesHighToLow.end(); priority_it++)
			{
				con::mpmc_queue<JobRecord>& queue = GetQueueForPriority(*priority_it);
				while (queue.try_dequeue(record)) {}
			}

			_job_workers.clear();
			_thread_pool.reset();
			_job_pool.Clear();
		}

		void SetDefaultJobWorkerCount()
		{
			// we want to be sure we use one less the number of cores available so our main thread is not crowded
			SetJobWorkerCount(thr::Thread::HardwareConcurrency() - 1);
		}

		/*
			This method also calls Dispose, so setup jobs after calling this
		*/
		void SetJobWorkerCount(siz count)
		{
			Dispose();
			_thread_pool = mem::create_sptr<thr::ThreadPool>(_thread_pool_allocator, count);

			while (_job_workers.size() < count)
				_job_workers.emplace_back(*this);

			while (_job_workers.size() > count)
				_job_workers.pop_back();

			for (auto it1 = _job_workers.begin(); it1 != _job_workers.end(); it1++)
			{
				it1->ClearCoworkers();
				for (auto it2 = _job_workers.begin(); it2 != _job_workers.end(); it2++)
					it1->AddCoworker(*it2);
			}
		}

		void Start()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			if (!_thread_pool)
				SetDefaultJobWorkerCount();

			_running.store(true, mo_release);

			for (siz i = 0; i < _job_workers.size(); i++)
				_job_workers[i].StartWork(*_thread_pool, (i + 1) % _thread_pool->ObjectCount());
		}

		void Stop()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			if (IsRunning())
			{
				for (auto it = _job_workers.begin(); it != _job_workers.end(); it++)
					it->StopWork();

				_running.store(false, mo_release);
			}
		}

		bl IsRunning() const
		{
			return _running.load(mo_acquire);
		}

		con::mpmc_queue<JobRecord>& GetQueueForPriority(JobPriority priority)
		{
			con::mpmc_queue<JobRecord>* deque = nullptr;

			switch (priority)
			{
			case JobPriority::Highest:
				deque = &_highest_job_queue;
				break;

			case JobPriority::Higher:
				deque = &_higher_job_queue;
				break;

			case JobPriority::Normal:
				deque = &_normal_job_queue;
				break;

			case JobPriority::Lower:
				deque = &_lower_job_queue;
				break;

			case JobPriority::Lowest:
				deque = &_lowest_job_queue;
				break;

			default:
				NP_ENGINE_ASSERT(false, "requested incorrect priority");
				break;
			}

			return *deque;
		}

		JobRecord SubmitJob(JobPriority priority, mem::sptr<Job> job)
		{
			return SubmitJob({priority, job});
		}

		JobRecord SubmitJob(JobRecord record)
		{
			NP_ENGINE_ASSERT(record.IsValid(), "attempted to add an invalid Job -- do not do that my guy");
			NP_ENGINE_ASSERT(record.job->IsEnabled(), "the dude not enabled bro - why it do");

			JobRecord return_record;

			if (GetQueueForPriority(record.priority).enqueue(record))
				return_record = record;

			return return_record;
		}

		mem::sptr<Job> CreateJob()
		{
			return _job_pool.CreateObject();
		}

		con::vector<JobWorker>& GetJobWorkers()
		{
			return _job_workers;
		}

		const con::vector<JobWorker>& GetJobWorkers() const
		{
			return _job_workers;
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_SYSTEM_IMPL_HPP */
