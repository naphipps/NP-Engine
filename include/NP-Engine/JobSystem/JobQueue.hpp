//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/1/23
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_QUEUE_HPP
#define NP_ENGINE_JOB_QUEUE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "JobRecord.hpp"
#include "Job.hpp"
#include "JobPriority.hpp"

namespace np::jsys
{
	class JobQueue
	{
	private:
		using JobsQueue = mutexed_wrapper<con::queue<JobRecord>>;
		con::array<JobsQueue, (siz)JobPriority::Max> _jobs_queues;

		JobsQueue& GetQueueForPriority(JobPriority priority)
		{
			return _jobs_queues[(siz)priority];
		}

	public:
		void Push(JobPriority priority, mem::sptr<Job> job)
		{
			Push({priority, job});
		}

		void Push(JobRecord record)
		{
			NP_ENGINE_ASSERT(record.IsValid(), "attempted to add an invalid Job -- do not do that my guy");
			NP_ENGINE_ASSERT(!record.job->IsComplete(), "the dude is complete bro - why it be");
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
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_QUEUE_HPP */