//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/23/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NSIT_TRACE_EVENT_HPP
#define NP_ENGINE_NSIT_TRACE_EVENT_HPP

#include <string>
#include <thread>

#include "NP-Engine/Time/Time.hpp"

namespace np::nsit
{
	struct trace_event
	{
		::std::string name{};
		tim::steady_timestamp start_timestamp{};
		tim::microseconds elapsed_microseconds{};
		tim::milliseconds elapsed_milliseconds{};
		::std::thread::id thread_id{};
	};
} // namespace np::nsit

#endif /* NP_ENGINE_NSIT_TRACE_EVENT_HPP */