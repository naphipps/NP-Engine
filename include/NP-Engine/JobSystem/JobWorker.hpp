//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_WORKER_HPP
#define NP_ENGINE_JOB_WORKER_HPP

#ifndef NP_ENGINE_JOB_WORKER_SLEEP_DURATION
	#define NP_ENGINE_JOB_WORKER_SLEEP_DURATION 1 //milliseconds
#endif

#ifndef NP_ENGINE_JOB_WORKER_DEEP_SLEEP_DURATION
	#define NP_ENGINE_JOB_WORKER_DEEP_SLEEP_DURATION 3 //milliseconds
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
		mem::sptr<thr::Thread> _thread;
		JobQueue& _job_queue;
		mutexed_wrapper<con::queue<mem::sptr<Job>>> _immediate_jobs;
		rng::Random64 _rng;
		mutexed_wrapper<con::vector<JobWorker*>> _coworkers;
		siz _coworker_index;
		mutexed_wrapper<FetchOrderArray> _fetch_order;
		atm<::std::optional<ui32>> _deep_sleep_threshold;

		/*
			returns a valid && CanExecute() job, or invalid job
		*/
		mem::sptr<Job> GetImmediateJob(bl stealing = false)
		{
			mem::sptr<Job> job = nullptr;
			auto immediate_jobs = _immediate_jobs.get_access();
			if (!immediate_jobs->empty())
			{
				job = immediate_jobs->front();
				if (!job->CanExecute())
				{
					immediate_jobs->pop();
					immediate_jobs->emplace(job);
					job.reset();
				}
				else if (stealing && !job->CanBeStolen())
				{
					job.reset();
				}
				else
				{
					immediate_jobs->pop();
				}
			}
			return job;
		}

		mem::sptr<Job> GetStolenJob()
		{
			auto coworkers = _coworkers.get_access();
			return coworkers->empty() ? nullptr : (*coworkers)[_coworker_index]->GetImmediateJob(true);
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
							_job_queue.Push(NormalizePriority(next_job.priority), next_job.job);
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
						::std::optional<ui32> deep_sleep_threshold = _deep_sleep_threshold.load(mo_acquire);
						if (!deep_sleep_threshold.has_value())
							deep_sleep_threshold = (ui32)(_coworkers.get_access()->size() * 3);

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
						_job_queue.Push(NormalizePriority(record.priority), record.job);

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
					_coworker_index = (_coworker_index + 1) % _coworkers.get_access()->size();
				}
			}

			return success;
		}

	public:
		JobWorker(JobQueue& job_queue):
			_keep_working(false),
			_thread(nullptr),
			_job_queue(job_queue),
			_coworker_index(0)
		{
			SetFetchOrder({ Fetch::Immediate, Fetch::PriorityBased, Fetch::Steal });
		}

		JobWorker(JobWorker&& other) noexcept :
			_keep_working(::std::move(other._keep_working.load(mo_acquire))),
			_thread(::std::move(other._thread)),
			_job_queue(other._job_queue),
			_rng(::std::move(other._rng)),
			_coworker_index(::std::move(other._coworker_index))
		{
			_coworkers.get_access()->swap(*other._coworkers.get_access());
			_immediate_jobs.get_access()->swap(*other._immediate_jobs.get_access());
			_fetch_order.get_access()->swap(*other._fetch_order.get_access());
		}

		virtual ~JobWorker()
		{
			StopWork();
		}

		void AddCoworker(JobWorker& coworker)
		{
			// intentionally not checking if we have this coworker already
			//	^ allows us to support uneven distribution when stealing jobs
			_coworkers.get_access()->emplace_back(mem::AddressOf(coworker));
		}

		void RemoveCoworker(JobWorker& coworker)
		{
			// because of potential uneven distrubutions for stealing jobs
			//	^ we have to iterate through all _coworkers
			auto coworkers = _coworkers.get_access();
			for (auto it = coworkers->begin(); it != coworkers->end();)
			{
				if (mem::AddressOf(coworker) == *it)
					it = coworkers->erase(it);
				else
					it++;
			}
		}

		void ClearCoworkers()
		{
			_coworkers.get_access()->clear();
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

		void SubmitImmediateJob(mem::sptr<Job> job)
		{
			NP_ENGINE_ASSERT((bl)job, "attempted to add an invalid Job -- do not do that my guy");
			NP_ENGINE_ASSERT(job->IsEnabled(), "the dude not enabled bro - why it do");
			_immediate_jobs.get_access()->push(job);
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
			_coworker_index = _rng.GetLemireWithinRange(_coworkers.get_access()->size());
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
