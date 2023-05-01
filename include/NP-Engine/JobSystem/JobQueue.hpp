//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/1/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_QUEUE_HPP
#define NP_ENGINE_JOB_QUEUE_HPP

#include "NP-Engine/Container/Container.hpp"

#include "JobRecord.hpp"
#include "Job.hpp"
#include "JobPriority.hpp"

namespace np::jsys
{
	class JobQueue
	{
	private:
		//TODO: test if this mutex wrapper is faster than moodycamel's concurrent queue
		using JobsQueue = mutexed_wrapper<con::queue<JobRecord>>;
		con::array<JobsQueue, 5> _jobs_queues;

		JobsQueue& GetQueueForPriority(JobPriority priority)
		{
			JobsQueue* queue = nullptr;
			switch (priority)
			{
			case JobPriority::Highest:
				queue = &_jobs_queues[0];
				break;

			case JobPriority::Higher:
				queue = &_jobs_queues[1];
				break;

			case JobPriority::Normal:
				queue = &_jobs_queues[2];
				break;

			case JobPriority::Lower:
				queue = &_jobs_queues[3];
				break;

			case JobPriority::Lowest:
				queue = &_jobs_queues[4];
				break;

			default:
				NP_ENGINE_ASSERT(false, "requested incorrect priority");
				break;
			}
			return *queue;
		}

	public:
		void Emplace(JobPriority priority, mem::sptr<Job> job)
		{
			Emplace({ priority, job });
		}

		void Emplace(JobRecord record)
		{
			NP_ENGINE_ASSERT(record.IsValid(), "attempted to add an invalid Job -- do not do that my guy");
			NP_ENGINE_ASSERT(record.job->IsEnabled(), "the dude not enabled bro - why it do");
			GetQueueForPriority(record.priority).get_access()->emplace(record);
		}

		JobRecord Pop(JobPriority priority)
		{
			JobRecord record;
			auto queue = GetQueueForPriority(priority).get_access();
			if (!queue->empty())
			{
				record = queue->front();
				queue->pop();
			}
			return record;
		}

		void Clear()
		{
			for (auto it = _jobs_queues.begin(); it != _jobs_queues.end(); it++)
			{
				auto queue = it->get_access();
				while (!queue->empty())
					queue->pop();
			}
		}
	};
}

#endif /* NP_ENGINE_JOB_QUEUE_HPP */