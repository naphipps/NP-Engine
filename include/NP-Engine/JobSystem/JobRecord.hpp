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
#include "NP-Engine/Memory/Memory.hpp"

#include "Job.hpp"
#include "JobPriority.hpp"

namespace np::jsys
{
	struct JobRecord
	{
		JobPriority priority = JobPriority::Normal;
		mem::sptr<Job> job = nullptr;

		bl IsValid() const
		{
			return (bl)job;
		}

		void Invalidate()
		{
			priority = JobPriority::Normal;
			job.reset();
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_RECORD_HPP */
