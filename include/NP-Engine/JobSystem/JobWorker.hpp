//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_WORKER_HPP
#define NP_ENGINE_JOB_WORKER_HPP

#ifndef NP_ENGINE_JOB_WORKER_SLEEP_DURATION
	#define NP_ENGINE_JOB_WORKER_SLEEP_DURATION 1 // milliseconds
#endif

#ifndef NP_ENGINE_JOB_WORKER_DEEP_SLEEP_DURATION
	#define NP_ENGINE_JOB_WORKER_DEEP_SLEEP_DURATION 3 // milliseconds
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
	class JobSystem;

	class JobWorker
	{
	private:
		struct WorkPayload
		{
			JobWorker* self = nullptr;
			JobSystem* system = nullptr;
		};

		siz _id;
		atm_bl _keep_working;
		atm_i64 _wake_counter;
		mem::sptr<thr::Thread> _thread;
		condition _sleep_condition;

		static void WorkProcedure(const WorkPayload& payload);

		bl ShouldSleep()
		{
			return _wake_counter.fetch_add(-1) == 1;
		}

		void Sleep()
		{
			mutex m;
			general_lock l(m);
			_sleep_condition.wait(l, [this]()
				{
					return !IsAwake();
				});

			WakeUp();
		}

	public:
		JobWorker(siz id): _id(id), _keep_working(false), _wake_counter(-1/* arbitrary */), _thread(nullptr)
		{}

		JobWorker(JobWorker&& other) noexcept:
			_id(::std::move(other._id)),
			_keep_working(::std::move(other._keep_working.load(mo_acquire))),
			_wake_counter(::std::move(other._wake_counter.load(mo_acquire))),
			_thread(::std::move(other._thread))
		{}

		virtual ~JobWorker()
		{
			StopWork();
		}

		void StartWork(JobSystem& system);

		void StopWork()
		{
			NP_ENGINE_PROFILE_FUNCTION();
			_keep_working.store(false, mo_release);
			_wake_counter.store(-1, mo_release);
			WakeUp();
			_thread.reset();
		}

		void WakeUp()
		{
			_wake_counter.fetch_add(1);
			_sleep_condition.notify_all();
		}

		bl IsAwake() const
		{
			return _keep_working.load(mo_acquire) && _wake_counter.load(mo_acquire) > 0;
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_WORKER_HPP */
