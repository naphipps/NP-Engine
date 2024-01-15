//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_WORKER_HPP
#define NP_ENGINE_JOB_WORKER_HPP

#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Thread/Thread.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "JobRecord.hpp"
#include "JobQueue.hpp"

namespace np::jsys
{
	class JobSystem;

	class JobWorker
	{
	private:
		friend class JobSystem;

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

	public:
		JobWorker(siz id):
			_id(id),
			_keep_working(false),
			_wake_counter(LOWEST_AWAKE_STATE),
			_thread(nullptr),
			_sleep_condition(nullptr)
		{}

		JobWorker(JobWorker&& other) noexcept:
			_id(::std::move(other._id)),
			_keep_working(::std::move(other._keep_working.load(mo_acquire))),
			_wake_counter(::std::move(other._wake_counter.load(mo_acquire))),
			_thread(::std::move(other._thread)),
			_sleep_condition(::std::move(other._sleep_condition))
		{}

		virtual ~JobWorker()
		{
			StopWork();
		}

		void StartWork(JobSystem& system);

		void StopWork()
		{
			NP_ENGINE_PROFILE_FUNCTION();

			bl was_working = _keep_working.exchange(false, mo_release);
			if (was_working)
			{
				_sleep_condition->notify_all();
				_thread.reset();
				_sleep_condition.reset();
			}
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_WORKER_HPP */
