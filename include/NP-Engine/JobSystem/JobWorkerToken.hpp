//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/9/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_WORKER_TOKEN_HPP
#define NP_ENGINE_JOB_WORKER_TOKEN_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "JobWorker.hpp"
#include "JobToken.hpp"

namespace np::js
{
	class JobWorkerToken
	{
	private:
		JobWorker* _worker;

	public:
		JobWorkerToken(): JobWorkerToken(nullptr) {}

		JobWorkerToken(JobWorker* worker): _worker(worker) {}

		JobWorkerToken(JobWorkerToken& other): _worker(other._worker) {}

		JobWorkerToken(JobWorkerToken&& other): _worker(other._worker) {}

		JobWorkerToken& operator=(const JobWorkerToken& other)
		{
			_worker = other._worker;
			return *this;
		}

		JobWorkerToken& operator=(JobWorkerToken&& other)
		{
			_worker = other._worker;
			return *this;
		}

		bl IsValid() const
		{
			return _worker != nullptr;
		}

		void Invalidate()
		{
			_worker = nullptr;
		}

		JobToken CreateJobToken(Job::Function& function, JobToken dependent = JobToken())
		{
			JobToken token;

			if (IsValid())
			{
				token = _worker->CreateJobToken(function, dependent);
			}

			return token;
		}

		JobRecord AddJob(JobPriority priority, JobToken token)
		{
			JobRecord record;

			if (IsValid())
			{
				record = _worker->AddJob(priority, token);
			}

			return record;
		}

		JobRecord AddJob(JobPriority priority, Job::Function function, JobToken dependent = JobToken())
		{
			JobRecord record;

			if (IsValid())
			{
				record = _worker->AddJob(priority, function, dependent);
			}

			return record;
		}

		explicit operator bl() const
		{
			return IsValid();
		}
	};
} // namespace np::js

#endif /* NP_ENGINE_JOB_WORKER_TOKEN_HPP */
