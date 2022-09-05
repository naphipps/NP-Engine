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

	private:
		atm_bl _keep_working;
		atm_bl _work_procedure_complete;
		thr::Thread* _thread;
		thr::ThreadPool* _thread_pool;
		JobSystem* _job_system;
		con::mpmc_queue<Job*> _immediate_job_queue;
		rng::Random64 _random_engine;
		con::vector<JobWorker*> _coworkers;
		siz _coworker_index;
		atm<con::array<Fetch, 3>> _fetch_order;

		/*
			returns a valid && CanExecute() job, or invalid job
		*/
		Job* GetImmediateJob()
		{
			Job* job = nullptr;
			if (_immediate_job_queue.try_dequeue(job) && !job->CanExecute())
			{
				SubmitImmediateJob(job);
				job = nullptr;
			}
			return job;
		}

		Job* GetStolenJob()
		{
			return _coworkers.empty() ? nullptr : _coworkers[_coworker_index]->GetImmediateJob();
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
		JobWorker():
			_keep_working(false),
			_work_procedure_complete(true),
			_thread(nullptr),
			_thread_pool(nullptr),
			_job_system(nullptr),
			_coworker_index(0),
			_fetch_order({Fetch::Immediate, Fetch::PriorityBased, Fetch::Steal})
		{}

		JobWorker(const JobWorker& other) = delete;

		JobWorker(JobWorker&& other) noexcept:
			_keep_working(::std::move(other._keep_working.load(mo_acquire))),
			_work_procedure_complete(::std::move(other._work_procedure_complete.load(mo_acquire))),
			_thread(::std::move(other._thread)),
			_thread_pool(::std::move(other._thread_pool)),
			_job_system(::std::move(other._job_system)),
			_immediate_job_queue(::std::move(other._immediate_job_queue)),
			_random_engine(::std::move(other._random_engine)),
			_coworkers(::std::move(other._coworkers)),
			_coworker_index(::std::move(other._coworker_index)),
			_fetch_order(::std::move(other._fetch_order.load(mo_acquire)))
		{}

		virtual ~JobWorker()
		{
			StopWork();
		}

		JobWorker& operator=(const JobWorker& other) = delete;

		JobWorker& operator=(JobWorker&& other) noexcept
		{
			_keep_working.store(::std::move(other._keep_working.load(mo_acquire)), mo_release);
			_work_procedure_complete.store(::std::move(other._work_procedure_complete.load(mo_acquire)), mo_release);
			_thread = ::std::move(other._thread);
			_thread_pool = ::std::move(other._thread_pool);
			_job_system = ::std::move(other._job_system);
			_immediate_job_queue = ::std::move(other._immediate_job_queue);
			_random_engine = ::std::move(other._random_engine);
			_coworkers = ::std::move(other._coworkers);
			_coworker_index = ::std::move(other._coworker_index);
			_fetch_order = ::std::move(other._fetch_order.load(mo_acquire));
			return *this;
		}

		void AddCoworker(JobWorker* coworker)
		{
			// intentionally not checking if we have this coworker already
			//	^ allows us to support uneven distribution when stealing jobs
			if (coworker != nullptr && coworker != this)
				_coworkers.emplace_back(coworker);
		}

		void RemoveCoworker(JobWorker* coworker)
		{
			// because of potential uneven distrubutions for stealing jobs, we have to iterate through all _coworkers
			for (auto it = _coworkers.begin(); it != _coworkers.end(); it++)
				if (coworker == *it)
					_coworkers.erase(it);
		}

		void ClearCoworkers()
		{
			_coworkers.clear();
		}

		Job* SubmitImmediateJob(Job* job)
		{
			NP_ENGINE_ASSERT(job != nullptr, "attempted to add an invalid Job -- do not do that my guy");
			NP_ENGINE_ASSERT(job->IsEnabled(), "the dude not enabled bro - why it do");

			Job* return_job = nullptr;

			if (_immediate_job_queue.enqueue(job))
				return_job = job;

			return return_job;
		}

		con::array<Fetch, 3> GetFetchOrder()
		{
			return _fetch_order.load(mo_acquire);
		}

		void SetFetchOrder(const con::array<Fetch, 3>& fetch_order)
		{
			_fetch_order.store(fetch_order, mo_release);
		}

		void StartWork(JobSystem& job_system, thr::ThreadPool& pool, siz thread_affinity)
		{
			NP_ENGINE_PROFILE_FUNCTION();
			_job_system = mem::AddressOf(job_system);
			_thread_pool = mem::AddressOf(pool);
			_keep_working.store(true, mo_release);
			_work_procedure_complete.store(false, mo_release);
			_coworker_index = _random_engine.GetLemireWithinRange(_coworkers.size());
			_thread = _thread_pool->CreateObject();
			_thread->Run(&JobWorker::WorkerThreadProcedure, this);
			_thread->SetAffinity(thread_affinity);
		}

		void StopWork()
		{
			NP_ENGINE_PROFILE_FUNCTION();
			_keep_working.store(false, mo_release);

			while (!_work_procedure_complete.load(mo_acquire))
				thr::ThisThread::yield();

			if (_thread_pool && _thread)
			{
				_thread_pool->DestroyObject(_thread);
				_thread_pool = nullptr;
				_thread = nullptr;
			}
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_WORKER_HPP */
