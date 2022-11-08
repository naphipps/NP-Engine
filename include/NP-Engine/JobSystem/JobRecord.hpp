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
	struct JobRecord
	{
		JobPriority priority = JobPriority::Normal;
		Job* job = nullptr;

		bl IsValid() const
		{
			return job != nullptr;
		}

		void Invalidate()
		{
			job = nullptr;
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_RECORD_HPP */
