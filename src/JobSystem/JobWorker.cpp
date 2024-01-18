//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/14/24
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/String/String.hpp" //just for insight usage

#include "NP-Engine/JobSystem/JobWorker.hpp"
#include "NP-Engine/JobSystem/JobSystem.hpp"

namespace np::jsys
{
	void JobWorker::WorkProcedure(const WorkPayload& payload)
	{
		NP_ENGINE_PROFILE_SCOPE("WorkerThreadProcedure: " + to_str(payload.self->_id));

		JobWorker& self = *payload.self;
		JobSystem& system = *payload.system;
		mutex sleep_mutex;
		general_lock sleep_lock(sleep_mutex);
		IsAwakeFunctor is_awake_functor{self};

		while (self._keep_working.load(mo_acquire))
		{
			if (!(self.TryImmediateJob() || self.TryPriorityBasedJob(system) || self.TryStealingJob()) && self.ShouldSleep())
				self._sleep_condition->wait(sleep_lock, ::std::ref(is_awake_functor)); // preference: no lambda - ew

			self.ResetWakeCounter();
		}
	}

	bl JobWorker::TryPriorityBasedJob(JobSystem& system)
	{
		JobRecord next = system.GetNextJob();
		if (next.IsValid())
		{
			if (next.job->CanExecute())
				(*next.job)(_id);
			else
				system.SubmitJob(NormalizePriority(next.priority), next.job);
		}
		return next.IsValid();
	}

	void JobWorker::StartWork(JobSystem& system)
	{
		bl was_working = _keep_working.exchange(true, mo_release);
		NP_ENGINE_ASSERT(!was_working, "JobWorker is already working.");

		ResetWakeCounter();
		_sleep_condition = system.GetJobWorkerSleepCondition();
		_thread = system.CreateThread();
		_thread->Run(WorkProcedure, WorkPayload{this, &system});
		_thread->SetAffinity(system.GetThreadAffinity(_id));
	}
} // namespace np::jsys