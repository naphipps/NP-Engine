//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/23/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_INSTRUMENTOR_TIMER_HPP
#define NP_ENGINE_INSTRUMENTOR_TIMER_HPP

#include <string>
#include <thread>

#include "TraceEvent.hpp"
#include "ScopedTimer.hpp"
#include "Instrumentor.hpp"

namespace np::nsit
{
	class InstrumentorTimer : public ScopedTimer
	{
	private:
		static void AddTraceEventAction(ScopedTimer& timer)
		{
			TraceEvent e;
			e.Name = timer.GetName();
			e.ElapsedMilliseconds = timer.GetElapsedMilliseconds();
			e.ElapsedMicroseconds = timer.GetElapsedMicroseconds();
			e.StartTimestamp = timer.GetStartTimestamp();
			e.ThreadId = ::std::this_thread::get_id();
			Instrumentor::AddTraceEvent(e);
		}

	public:
		InstrumentorTimer(::std::string name): ScopedTimer(name, AddTraceEventAction) {}
	};
} // namespace np::nsit

#endif /* NP_ENGINE_INSTRUMENTOR_TIMER_HPP */