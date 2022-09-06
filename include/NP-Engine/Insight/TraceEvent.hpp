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

namespace np::nsit
{
	struct TraceEvent
	{
		::std::string Name;
		tim::SteadyTimestamp StartTimestamp;
		tim::DblMicroseconds ElapsedMicroseconds;
		tim::DblMilliseconds ElapsedMilliseconds;
		::std::thread::id ThreadId;
	};
} // namespace np::nsit

#endif /* NP_ENGINE_TRACE_EVENT_HPP */