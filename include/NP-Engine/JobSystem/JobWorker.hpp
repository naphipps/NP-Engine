//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_WORKER_HPP
#define NP_ENGINE_JOB_WORKER_HPP

#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Random/Random.hpp"
#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/String/String.hpp"

#include "JobRecord.hpp"
#include "JobPool.hpp"

namespace np::jsys
{
	class JobSystem;

	class JobWorker
	{
	private:
		friend class JobSystem;

	public:
		enum class Fetch : ui32
		{
			None = 0,
			Immediate = BIT(0),
			PriorityBased = BIT(1),
			Steal = BIT(2)
		};

		using FetchOrderArray = con::array<Fetch, 3>;

	private:
		atm_bl _keep_working;
		atm_bl _work_procedure_complete;
		mem::sptr<thr::Thread> _thread;
		JobSystem& _job_system;
		con::mpmc_queue<mem::sptr<Job>> _immediate_job_queue;
		rng::Random64 _random_engine;
		con::vector<JobWorker*> _coworkers;
		siz _coworker_index;
		Mutex _fetch_order_mutex;
		FetchOrderArray _fetch_order;

		/*
			returns a valid && CanExecute() job, or invalid job
		*/
		mem::sptr<Job> GetImmediateJob(bl stealing = false)
		{
			mem::sptr<Job> job = nullptr;
			if (_immediate_job_queue.try_dequeue(job) && (!job->CanExecute() || (stealing && !job->CanBeStolen())))
			{
				SubmitImmediateJob(job);
				job.Reset();
			}
			return job;
		}

		mem::sptr<Job> GetStolenJob()
		{
			return _coworkers.empty() ? nullptr : _coworkers[_coworker_index]->GetImmediateJob(true);
		}

		/*
			returns a valid && CanExecute() job, or invalid job
		*/
		JobRecord GetNextJob();

		void WorkerThreadProcedure();

		bl TryImmediateJob();

		bl TryPriorityBasedJob();

		bl TryStealingJob();

	public:
		JobWorker(JobSystem& job_system):
			_keep_working(false),
			_work_procedure_complete(true),
			_thread(nullptr),
			_job_system(job_system),
			_coworker_index(0),
			_fetch_order({Fetch::Immediate, Fetch::PriorityBased, Fetch::Steal})
		{}

		JobWorker(const JobWorker& other) = delete;

		JobWorker(JobWorker&& other) noexcept:
			_keep_working(::std::move(other._keep_working.load(mo_acquire))),
			_work_procedure_complete(::std::move(other._work_procedure_complete.load(mo_acquire))),
			_thread(::std::move(other._thread)),
			_job_system(other._job_system),
			_immediate_job_queue(::std::move(other._immediate_job_queue)),
			_random_engine(::std::move(other._random_engine)),
			_coworkers(::std::move(other._coworkers)),
			_coworker_index(::std::move(other._coworker_index)),
			_fetch_order(::std::move(other._fetch_order))
		{}

		virtual ~JobWorker()
		{
			StopWork();
		}

		JobWorker& operator=(const JobWorker& other) = delete;

		JobWorker& operator=(JobWorker&& other) noexcept = delete;

		void AddCoworker(JobWorker& coworker)
		{
			// intentionally not checking if we have this coworker already
			//	^ allows us to support uneven distribution when stealing jobs
			JobWorker* coworker_ptr = mem::AddressOf(coworker);
			if (coworker_ptr != this)
				_coworkers.emplace_back(coworker_ptr);
		}

		void RemoveCoworker(JobWorker& coworker)
		{
			// because of potential uneven distrubutions for stealing jobs, we have to iterate through all _coworkers
			for (auto it = _coworkers.begin(); it != _coworkers.end(); it++)
				if (mem::AddressOf(coworker) == *it)
					_coworkers.erase(it);
		}

		void ClearCoworkers()
		{
			_coworkers.clear();
		}

		bl SubmitImmediateJob(mem::sptr<Job> job)
		{
			NP_ENGINE_ASSERT((bl)job, "attempted to add an invalid Job -- do not do that my guy");
			NP_ENGINE_ASSERT(job->IsEnabled(), "the dude not enabled bro - why it do");
			return _immediate_job_queue.enqueue(job);
		}

		FetchOrderArray GetFetchOrder()
		{
			Lock lock(_fetch_order_mutex);
			return _fetch_order;
		}

		void SetFetchOrder(const FetchOrderArray& fetch_order)
		{
			Lock lock(_fetch_order_mutex);
			_fetch_order = fetch_order;
		}

		void StartWork(thr::ThreadPool& thread_pool, siz thread_affinity)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			_keep_working.store(true, mo_release);
			_work_procedure_complete.store(false, mo_release);
			_coworker_index = _random_engine.GetLemireWithinRange(_coworkers.size());
			_thread = thread_pool.CreateObject();
			_thread->Run(&JobWorker::WorkerThreadProcedure, this);
			_thread->SetAffinity(thread_affinity);
		}

		void StopWork()
		{
			NP_ENGINE_PROFILE_FUNCTION();
			_keep_working.store(false, mo_release);

			while (!_work_procedure_complete.load(mo_acquire))
				thr::ThisThread::yield();

			_thread.Reset();
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_WORKER_HPP */
