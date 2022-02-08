//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/31/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_RECORD_HPP
#define NP_ENGINE_JOB_RECORD_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Job.hpp"
#include "JobPriority.hpp"

namespace np::js
{
	class JobRecord
	{
	private:
		JobPriority _priority;
		Job* _job;

	public:
		JobRecord(): JobRecord(JobPriority::Normal, nullptr) {}

		JobRecord(JobPriority priority, const Job* job): _priority(priority), _job(const_cast<Job*>(job)) {}

		JobRecord(const JobRecord& record): JobRecord(record._priority, record._job) {}

		JobRecord(JobRecord&& record): JobRecord(record._priority, record._job) {}

		JobRecord& operator=(const JobRecord& other)
		{
			_priority = other._priority;
			_job = other._job;
			return *this;
		}

		JobRecord& operator=(JobRecord&& other)
		{
			_priority = other._priority;
			_job = other._job;
			return *this;
		}

		bl IsValid() const
		{
			return _job != nullptr;
		}

		void Invalidate()
		{
			_job = nullptr;
		}

		const Job& GetJob() const
		{
			NP_ASSERT(IsValid(), "do not call this if we have an invalid job");

			return *_job;
		}

		void Execute()
		{
			NP_ASSERT(IsValid(), "do not call this if we have an invalid job");

			_job->Execute();
		}

		JobPriority GetPriority() const
		{
			return _priority;
		}

		explicit operator bl() const
		{
			return IsValid();
		}
	};
} // namespace np::js

#endif /* NP_ENGINE_JOB_RECORD_HPP */
