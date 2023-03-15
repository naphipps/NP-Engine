//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/31/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_PRIORITY_HPP
#define NP_ENGINE_JOB_PRIORITY_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Insight/Insight.hpp"

namespace np::jsys
{
	enum class JobPriority : ui32
	{
		Highest,
		Higher,
		Normal,
		Lower,
		Lowest
	};

	static inline JobPriority NormalizePriority(JobPriority priority)
	{
		JobPriority return_priority = priority;

		switch (priority)
		{
		case JobPriority::Highest:
			return_priority = JobPriority::Higher;
			break;

		case JobPriority::Higher:
			return_priority = JobPriority::Normal;
			break;

		case JobPriority::Normal:
			return_priority = JobPriority::Normal;
			break;

		case JobPriority::Lower:
			return_priority = JobPriority::Normal;
			break;

		case JobPriority::Lowest:
			return_priority = JobPriority::Lower;
			break;

		default:
			NP_ENGINE_ASSERT(false, "requested incorrect priority");
			break;
		}

		return return_priority;
	}

	constexpr static con::array<JobPriority, 5> JobPrioritiesHighToLow{
		JobPriority::Highest, JobPriority::Higher, JobPriority::Normal, JobPriority::Lower, JobPriority::Lowest};

	constexpr static con::array<JobPriority, 5> JobPrioritiesLowToHigh{
		JobPriority::Lowest, JobPriority::Lower, JobPriority::Normal, JobPriority::Higher, JobPriority::Highest};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_PRIORITY_HPP */
