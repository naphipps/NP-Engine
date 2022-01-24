//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/6/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_TOKEN_HPP
#define NP_ENGINE_JOB_TOKEN_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "Job.hpp"

namespace np::js
{
	class JobToken
	{
	private:
		Job* _job;

	public:
		JobToken(Job* j = nullptr): _job(j) {}

		inline explicit operator bl() const
		{
			return IsValid();
		}

		inline bl IsValid() const
		{
			return _job != nullptr;
		}

		inline Job& GetJob()
		{
			NP_ASSERT(IsValid(), "we require a valid token when getting the job");
			return *_job;
		}

		inline const Job& GetJob() const
		{
			NP_ASSERT(IsValid(), "we require a valid token when getting the job");
			return *_job;
		}
	};
} // namespace np::js

#endif /* NP_ENGINE_JOB_TOKEN_HPP */
