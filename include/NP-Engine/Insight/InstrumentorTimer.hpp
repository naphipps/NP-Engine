//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/23/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NSIT_INSTRUMENTOR_TIMER_HPP
#define NP_ENGINE_NSIT_INSTRUMENTOR_TIMER_HPP

#include <string>
#include <thread>

#include "TraceEvent.hpp"
#include "ScopedTimer.hpp"
#include "Instrumentor.hpp"

namespace np::nsit
{
	class instrumentor_timer : public scoped_timer
	{
	private:
		static void add_trace_event_action(scoped_timer& timer)
		{
			trace_event e{};
			e.name = timer.get_name();
			e.elapsed_milliseconds = timer.get_elapsed_milliseconds();
			e.elapsed_microseconds = timer.get_elapsed_microseconds();
			e.start_timestamp = timer.get_start_timestamp();
			e.thread_id = ::std::this_thread::get_id();
			instrumentor::add_trace_event(e);
		}

	public:
		instrumentor_timer(::std::string name): scoped_timer(name, add_trace_event_action) {}
	};
} // namespace np::nsit

#endif /* NP_ENGINE_NSIT_INSTRUMENTOR_TIMER_HPP */