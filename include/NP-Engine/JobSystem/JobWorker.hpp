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
#include "JobQueue.hpp"

namespace np::jsys
{
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
		JobQueue& _job_queue;
		con::mpmc_queue<mem::sptr<Job>> _immediate_job_queue;
		rng::Random64 _random_engine;
		con::vector<JobWorker*> _coworkers;
		siz _coworker_index;
		mutexed_wrapper<FetchOrderArray> _fetch_order;

		/*
			returns a valid && CanExecute() job, or invalid job
		*/
		mem::sptr<Job> GetImmediateJob(bl stealing = false)
		{
			mem::sptr<Job> job = nullptr;
			if (_immediate_job_queue.try_dequeue(job) && (!job->CanExecute() || (stealing && !job->CanBeStolen())))
			{
				SubmitImmediateJob(job);
				job.reset();
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
							_job_queue.Emplace(NormalizePriority(next_job.priority), next_job.job);
							next_job.Invalidate(); //done with record
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

							while (tim::SteadyClock::now() - start < duration)
								thr::ThisThread::yield();

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

		bl TryImmediateJob()
		{
			bl success = !_keep_working.load(mo_acquire);

			if (!success)
			{
				mem::sptr<Job> immediate = GetImmediateJob();
				if (immediate)
				{
					NP_ENGINE_PROFILE_SCOPE("executing immediate Job");
					success = true;
					(*immediate)();
					if (!immediate->IsComplete())
						SubmitImmediateJob(immediate);

					immediate.reset(); //done with job
				}
			}

			return success;
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
					(*record.job)();

					if (!record.job->IsComplete())
						_job_queue.Emplace(NormalizePriority(record.priority), record.job);

					record.Invalidate(); //done with record
				}
			}

			return success;
		}

		bl TryStealingJob()
		{
			bl success = !_keep_working.load(mo_acquire);

			if (!success)
			{
				mem::sptr<Job> stolen = GetStolenJob();
				if (stolen)
				{
					NP_ENGINE_PROFILE_SCOPE("executing stolen Job");
					success = true;
					(*stolen)();
					if (!stolen->IsComplete())
						SubmitImmediateJob(stolen);

					stolen.reset(); //done with job
				}
				else
				{
					// we did not steal a good job thus we want to steal from someone else
					_coworker_index = (_coworker_index + 1) % _coworkers.size();
				}
			}

			return success;
		}

	public:
		JobWorker(JobQueue& job_queue):
			_keep_working(false),
			_work_procedure_complete(true),
			_thread(nullptr),
			_job_queue(job_queue),
			_coworker_index(0)
		{
			SetFetchOrder({ Fetch::Immediate, Fetch::PriorityBased, Fetch::Steal });
		}

		JobWorker(const JobWorker& other) = delete;

		JobWorker(JobWorker&& other) noexcept:
			_keep_working(::std::move(other._keep_working.load(mo_acquire))),
			_work_procedure_complete(::std::move(other._work_procedure_complete.load(mo_acquire))),
			_thread(::std::move(other._thread)),
			_job_queue(other._job_queue),
			_immediate_job_queue(::std::move(other._immediate_job_queue)),
			_random_engine(::std::move(other._random_engine)),
			_coworkers(::std::move(other._coworkers)),
			_coworker_index(::std::move(other._coworker_index))
		{
			SetFetchOrder(other.GetFetchOrder());
		}

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
			return *_fetch_order.get_access();
		}

		void SetFetchOrder(const FetchOrderArray& fetch_order)
		{
			*_fetch_order.get_access() = fetch_order;
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

			_thread.reset();
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_WORKER_HPP */
