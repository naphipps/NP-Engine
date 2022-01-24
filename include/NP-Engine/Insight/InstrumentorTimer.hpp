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

namespace np::insight
{
	class InstrumentorTimer : public ScopedTimer
	{
	private:
		static void AddTraceEventAction(ScopedTimer& timer)
		{
			TraceEvent event;
			event.Name = timer.GetName();
			event.ElapsedMilliseconds = timer.GetElapsedMilliseconds();
			event.ElapsedMicroseconds = timer.GetElapsedMicroseconds();
			event.StartTimestamp = timer.GetStartTimestamp();
			event.ThreadId = ::std::this_thread::get_id();
			np::insight::Instrumentor::Get().AddTraceEvent(event);
		}

	public:
		InstrumentorTimer(::std::string name): ScopedTimer(name, AddTraceEventAction) {}
	};
} // namespace np::insight

// check if NP_PROFILE_ENABLE is defined
#ifndef NP_PROFILE_ENABLE
	#define NP_PROFILE_ENABLE 0
#endif

#if NP_PROFILE_ENABLE
	#define NP_PROFILE_SCOPE(name) ::np::insight::InstrumentorTimer timer##__LINE__(name)
	#define NP_PROFILE_FUNCTION() NP_PROFILE_SCOPE(NP_FUNCTION)
#else
	#define NP_PROFILE_SCOPE(name)
	#define NP_PROFILE_FUNCTION()
#endif // NP_PROFILE_ENABLE

#endif /* NP_ENGINE_INSTRUMENTOR_TIMER_HPP */