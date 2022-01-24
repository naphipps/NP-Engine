//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/31/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_PRIORITY_HPP
#define NP_ENGINE_JOB_PRIORITY_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"

namespace np::js
{
	enum class JobPriority : ui32
	{
		HIGHEST,
		HIGHER,
		NORMAL,
		LOWER,
		LOWEST
	};

	static inline JobPriority NormalizePriority(JobPriority priority)
	{
		JobPriority return_priority = priority;

		switch (priority)
		{
		case JobPriority::HIGHEST:
			return_priority = JobPriority::HIGHER;
			break;

		case JobPriority::HIGHER:
			return_priority = JobPriority::NORMAL;
			break;

		case JobPriority::NORMAL:
			return_priority = JobPriority::NORMAL;
			break;

		case JobPriority::LOWER:
			return_priority = JobPriority::NORMAL;
			break;

		case JobPriority::LOWEST:
			return_priority = JobPriority::LOWER;
			break;

		default:
			NP_ASSERT(false, "requested incorrect priority");
			break;
		}

		return return_priority;
	}
} // namespace np::js

#endif /* NP_ENGINE_JOB_PRIORITY_HPP */
