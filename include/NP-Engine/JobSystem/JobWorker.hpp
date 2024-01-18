//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_WORKER_HPP
#define NP_ENGINE_JOB_WORKER_HPP

#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Thread/Thread.hpp"

#include "JobRecord.hpp"
#include "JobQueue.hpp"

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
			Immediate,
			PriorityBased,
			Steal
		};

		using FetchOrderArray = con::array<Fetch, 3>;

		constexpr static FetchOrderArray DEFAULT_FETCH_ORDER{Fetch::Immediate, Fetch::PriorityBased, Fetch::Steal};
		constexpr static FetchOrderArray IMMEDIATE_ONLY_FETCH_ORDER{Fetch::Immediate, Fetch::Immediate, Fetch::Immediate};
		constexpr static FetchOrderArray PRIORITY_BASED_ONLY_FETCH_ORDER{Fetch::PriorityBased, Fetch::PriorityBased, Fetch::PriorityBased};
		constexpr static FetchOrderArray STEAL_ONLY_FETCH_ORDER{Fetch::Steal, Fetch::Steal, Fetch::Steal};

	private:
		struct WorkPayload
		{
			JobWorker* self = nullptr;
			JobSystem* system = nullptr;
		};

		constexpr static siz SLEEP_STATE = 0;
		constexpr static siz LOWEST_AWAKE_STATE = 1;

		siz _id;
		atm_bl _keep_working;
		atm_siz _wake_counter;
		mem::sptr<thr::Thread> _thread;
		mem::sptr<condition> _sleep_condition;
		mutexed_wrapper<con::queue<mem::sptr<Job>>> _immediate_jobs;
		mutexed_wrapper<con::vector<JobWorker*>> _coworkers;
		mutexed_wrapper<FetchOrderArray> _fetch_order;

		static void WorkProcedure(const WorkPayload& payload);

		bl ShouldSleep()
		{
			return _wake_counter.fetch_sub(1) == LOWEST_AWAKE_STATE;
		}

		void WakeUp()
		{
			_wake_counter.fetch_add(1);
		}

		void ResetWakeCounter()
		{
			_wake_counter.store(LOWEST_AWAKE_STATE, mo_release);
		}

		bl IsAwake() const
		{
			// check if I can not sleep OR if my wake counter is above sleep state
			return !_keep_working.load(mo_acquire) || _wake_counter.load(mo_acquire) > SLEEP_STATE;
		}

		/*
			returns a valid && CanExecute() job, or invalid job
			iff a job was found AND we are not stealing, we increment our wake counter
		*/
		mem::sptr<Job> GetImmediateJob(JobWorker* thief = nullptr)
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

					// tell thief there are jobs to steal even though they are not ready
					if (thief && job->CanBeStolen())
						this->WakeUp();
					else // keep this worker awake to keep checking immediate jobs
						WakeUp();
						
					job.reset();
				}
				else if (thief && !job->CanBeStolen())
				{
					job.reset();
				}
				else
				{
					immediate_jobs->pop();

					// tell thief to stay awake in case there are more jobs ready to be stolen
					if (thief)
						thief->WakeUp();
				}
			}
			return job;
		}
		
		mem::sptr<Job> GetStolenJob()
		{
			mem::sptr<Job> job = nullptr;
			auto coworkers = _coworkers.get_access();
			for (siz i=0; i<coworkers->size() && !job; i++)
				job = (*coworkers)[i]->GetImmediateJob(this);

			return job;
		}

		bl TryImmediateJob(bl stealing = false)
		{
			mem::sptr<Job> immediate = GetImmediateJob();
			if (immediate)
				(*immediate)(_id);

			return immediate;
		}

		bl TryStealingJob()
		{
			mem::sptr<Job> stolen = GetStolenJob();
			if (stolen)
				(*stolen)(_id);

			return stolen;
		}

		/*
			returns true iff a job was found, else false
			FOUND JOB MAY OR MAY NOT BE EXECUTED
		*/
		bl TryPriorityBasedJob(JobSystem& system);

		void StartWork(JobSystem& system);

		void StopWork()
		{
			bl was_working = _keep_working.exchange(false, mo_release);
			if (was_working)
			{
				_sleep_condition->notify_all();
				_thread.reset();
				_sleep_condition.reset();
			}
		}

	public:
		JobWorker(siz id):
			_id(id),
			_keep_working(false),
			_wake_counter(LOWEST_AWAKE_STATE),
			_thread(nullptr),
			_sleep_condition(nullptr),
			_immediate_jobs(),
			_coworkers(),
			_fetch_order(DEFAULT_FETCH_ORDER)
		{}

		JobWorker(JobWorker&& other) noexcept:
			_id(::std::move(other._id)),
			_keep_working(::std::move(other._keep_working.load(mo_acquire))),
			_wake_counter(::std::move(other._wake_counter.load(mo_acquire))),
			_thread(::std::move(other._thread)),
			_sleep_condition(::std::move(other._sleep_condition)),
			_immediate_jobs(::std::move(other._immediate_jobs)),
			_coworkers(::std::move(other._coworkers)),
			_fetch_order(::std::move(other._fetch_order))
		{}

		virtual ~JobWorker()
		{
			StopWork();
		}

		void SubmitImmediateJob(mem::sptr<Job> job)
		{
			if (job && !job->IsComplete())
			{
				_immediate_jobs.get_access()->emplace(job);
				WakeUp();

				if (_sleep_condition)
					_sleep_condition->notify_all(); // ensure that I wake up - necessary evil
			}
		}

		FetchOrderArray GetFetchOrder()
		{
			return *_fetch_order.get_access();
		}

		void SetFetchOrder(const FetchOrderArray& fetch_order)
		{
			*_fetch_order.get_access() = fetch_order;
		}

		void AddCoworker(JobWorker& coworker)
		{
			// intentionally not checking if we have this coworker already
			//	^ allows us to support uneven distribution when stealing jobs
			_coworkers.get_access()->emplace_back(mem::AddressOf(coworker));
		}

		void RemoveCoworker(JobWorker& coworker)
		{
			// because of potential uneven distrubutions for stealing jobs,
			// we have to iterate through all _coworkers
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
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_WORKER_HPP */
