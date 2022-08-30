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
#include "NP-Engine/Math/Math.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "JobWorker.hpp"
#include "JobPool.hpp"

#ifndef NP_ENGINE_JOB_SYSTEM_POOL_DEFAULT_SIZE
	#define NP_ENGINE_JOB_SYSTEM_POOL_DEFAULT_SIZE 2000
#endif

namespace np::jsys
{
	class JobSystem
	{
	private:
		constexpr static siz JOB_ALIGNED_SIZE = mem::CalcAlignedSize(sizeof(Job));

		mem::TraitAllocator _allocator;
		atm_bl _running;

		con::vector<JobWorker> _job_workers;
		thr::ThreadPool _thread_pool;
		mem::Block _job_pool_block;
		JobPool* _job_pool;

		con::mpmc_queue<JobRecord> _highest_job_queue;
		con::mpmc_queue<JobRecord> _higher_job_queue;
		con::mpmc_queue<JobRecord> _normal_job_queue;
		con::mpmc_queue<JobRecord> _lower_job_queue;
		con::mpmc_queue<JobRecord> _lowest_job_queue;

	public:
		JobSystem(): _running(false)
		{
			_job_pool_block = _allocator.Allocate(JOB_ALIGNED_SIZE * NP_ENGINE_JOB_SYSTEM_POOL_DEFAULT_SIZE);
			_job_pool = mem::Create<JobPool>(_allocator, _job_pool_block);

			// we want to be sure we use one less the number of cores available so our main thread is not crowded
			_job_workers.resize(::std::min(_thread_pool.ObjectCount() - 1, thr::ThreadPool::MAX_THREAD_COUNT));

			for (auto it1 = _job_workers.begin(); it1 != _job_workers.end(); it1++)
				for (auto it2 = _job_workers.begin(); it2 != _job_workers.end(); it2++)
					it1->AddCoworker(mem::AddressOf(*it2));
		}

		~JobSystem()
		{
			Dispose();
		}

		void Dispose()
		{
			Stop();

			if (_job_pool)
			{
				mem::Destroy<JobPool>(_allocator, _job_pool);
				_job_pool = nullptr;
			}

			if (_job_pool_block.IsValid())
			{
				_allocator.Deallocate(_job_pool_block);
				_job_pool_block.Invalidate();
			}
		}

		void SetJobPoolSize(siz size)
		{
			Dispose();

			_job_pool_block = _allocator.Allocate(JOB_ALIGNED_SIZE * size);
			_job_pool = mem::Create<JobPool>(_allocator, _job_pool_block);
		}

		void Start()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			if (!_job_pool)
			{
				_job_pool_block = _allocator.Allocate(JOB_ALIGNED_SIZE * NP_ENGINE_JOB_SYSTEM_POOL_DEFAULT_SIZE);
				_job_pool = mem::Create<JobPool>(_allocator, _job_pool_block);
			}

			_running.store(true, mo_release);

			for (siz i = 0; i < _job_workers.size(); i++)
				_job_workers[i].StartWork(*this, _thread_pool, (i + 1) % _thread_pool.ObjectCount());
		}

		void Stop()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			if (IsRunning())
			{
				// stop workers
				for (auto it = _job_workers.begin(); it != _job_workers.end(); it++)
					it->StopWork();

				// stop threads
				_thread_pool.Clear();
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

		JobRecord SubmitJob(JobPriority priority, Job* job)
		{
			return SubmitJob(JobRecord(priority, job));
		}

		JobRecord SubmitJob(JobRecord record)
		{
			NP_ENGINE_ASSERT(record.IsValid(), "attempted to add an invalid Job -- do not do that my guy");
			NP_ENGINE_ASSERT(record.GetJob()->IsEnabled(), "the dude not enabled bro - why it do");

			JobRecord return_record;

			if (GetQueueForPriority(record.GetPriority()).enqueue(record))
				return_record = record;

			return return_record;
		}

		Job* CreateJob(mem::Delegate&& d)
		{
			return _job_pool->CreateObject(::std::move(d));
		}

		Job* CreateJob()
		{
			return _job_pool->CreateObject();
		}

		bl DestroyJob(Job* job)
		{
			return _job_pool->DestroyObject(job);
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
