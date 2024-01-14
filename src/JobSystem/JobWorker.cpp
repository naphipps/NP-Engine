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

		JobWorker& self = *payload.self;
		JobSystem& system = *payload.system;
		mutex sleep_mutex;
		general_lock sleep_lock(sleep_mutex);
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
				self.Sleep();
			}
		}
	}

	void JobWorker::StartWork(JobSystem& system)
	{
		NP_ENGINE_PROFILE_FUNCTION();
		_keep_working.store(true, mo_release);
		WakeUp();
		_thread = system.CreateThread();
		_thread->Run(WorkProcedure, WorkPayload{ this, &system });
		_thread->SetAffinity(system.GetThreadAffinity(_id));
	}
}