//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TIMER_HPP
#define NP_ENGINE_TIMER_HPP

#include <string>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"

namespace np::nsit
{
	class Timer
	{
	protected:
		::std::string _name;
		tim::SteadyTimestamp _start_timestamp;
		tim::SteadyTimestamp _end_timestamp;

	public:
		Timer(::std::string name = ""): _start_timestamp(tim::SteadyClock::now()), _name(name)
		{
			_end_timestamp = _start_timestamp;
		}

		virtual void Restart()
		{
			_start_timestamp = tim::SteadyClock::now();
			_end_timestamp = _start_timestamp;
		}

		virtual void Stop()
		{
			_end_timestamp = tim::SteadyClock::now();
		}

		tim::SteadyTimestamp GetStartTimestamp()
		{
			return _start_timestamp;
		}

		tim::SteadyTimestamp GetEndTimestamp()
		{
			return _end_timestamp;
		}

		void SetName(::std::string name)
		{
			_name = name;
		}

		::std::string GetName()
		{
			return _name;
		}

		tim::DblMilliseconds GetElapsedMilliseconds()
		{
			return _end_timestamp.time_since_epoch() - _start_timestamp.time_since_epoch();
		}

		tim::DblMilliseconds GetElapsedMicroseconds()
		{
			return _end_timestamp.time_since_epoch() - _start_timestamp.time_since_epoch();
		}
	};
} // namespace np::nsit

#endif /* NP_ENGINE_TIMER_HPP */
