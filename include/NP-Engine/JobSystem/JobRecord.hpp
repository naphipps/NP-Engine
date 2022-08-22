//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/31/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_RECORD_HPP
#define NP_ENGINE_JOB_RECORD_HPP

#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Job.hpp"
#include "JobPriority.hpp"

namespace np::jsys
{
	class JobRecord
	{
	private:
		JobPriority _priority;
		Job* _job;

	public:
		JobRecord(): JobRecord(JobPriority::Normal, nullptr) {}

		JobRecord(JobPriority priority, Job* job): _priority(priority), _job(job) {}

		JobRecord(const JobRecord& other): _priority(other._priority), _job(other._job) {}

		JobRecord(JobRecord&& other) noexcept: _priority(::std::move(other._priority)), _job(::std::move(other._job)) {}

		JobRecord& operator=(const JobRecord& other)
		{
			_priority = other._priority;
			_job = other._job;
			return *this;
		}

		JobRecord& operator=(JobRecord&& other) noexcept
		{
			_priority = ::std::move(other._priority);
			_job = ::std::move(other._job);
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

		Job* GetJob() const
		{
			return _job;
		}

		void Execute()
		{
			_job->Execute();
		}

		JobPriority GetPriority() const
		{
			return _priority;
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_RECORD_HPP */
