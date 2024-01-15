//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/14/24
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/JobSystem/JobWorker.hpp"
#include "NP-Engine/JobSystem/JobSystem.hpp"

namespace np::jsys
{
	void JobWorker::WorkProcedure(const JobWorker::WorkPayload& payload)
	{
		NP_ENGINE_PROFILE_SCOPE("WorkerThreadProcedure: " + to_str(payload.self->_id));

		struct IsAwakeFunctor
		{
			JobWorker& worker;

			bl operator()() const
			{
				return worker.IsAwake();
			}
		};

		JobWorker& self = *payload.self;
		JobSystem& system = *payload.system;
		mutex sleep_mutex;
		general_lock sleep_lock(sleep_mutex);
		IsAwakeFunctor is_awake_functor{self};
		JobRecord next;

		while (self._keep_working.load(mo_acquire))
		{
			next = system.GetNextJob();
			if (next.IsValid())
			{
				NP_ENGINE_ASSERT(!next.job->IsComplete(), "A found valid record must have an incomplete job");

				if (next.job->CanExecute())
					(*next.job)(self._id);
				else
					system.SubmitJob(NormalizePriority(next.priority), next.job);

				next.Invalidate();
			}
			else if (self.ShouldSleep())
			{
				self._sleep_condition->wait(sleep_lock, ::std::ref(is_awake_functor)); //preference: no lambda - ew
			}

			self.ResetWakeCounter();
		}
	}

	void JobWorker::StartWork(JobSystem& system)
	{
		NP_ENGINE_PROFILE_FUNCTION();

		bl was_working = _keep_working.exchange(true, mo_release);
		NP_ENGINE_ASSERT(!was_working, "Cannot call JobWorker.StartWork a second time without calling JobWorker.StopWork before");

		ResetWakeCounter();
		_sleep_condition = system.GetJobWorkerSleepCondition();
		_thread = system.CreateThread();
		_thread->Run(WorkProcedure, WorkPayload{ this, &system });
		_thread->SetAffinity(system.GetThreadAffinity(_id));
	}
}