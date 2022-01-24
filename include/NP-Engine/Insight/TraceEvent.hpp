//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/23/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TRACE_EVENT_HPP
#define NP_ENGINE_TRACE_EVENT_HPP

#include <string>
#include <thread>

#include "NP-Engine/Time/Time.hpp"

namespace np::insight
{
	struct TraceEvent
	{
		::std::string Name;
		time::SteadyTimestamp StartTimestamp;
		time::DurationMicroseconds ElapsedMicroseconds;
		time::DurationMilliseconds ElapsedMilliseconds;
		::std::thread::id ThreadId;
	};
} // namespace np::insight

#endif /* NP_ENGINE_TRACE_EVENT_HPP */