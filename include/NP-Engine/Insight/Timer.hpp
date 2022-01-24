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

namespace np::insight
{
	class Timer
	{
	protected:
		::std::string _name;
		time::SteadyTimestamp _start_timestamp;
		time::SteadyTimestamp _end_timestamp;

	public:
		Timer(::std::string name = ""): _start_timestamp(time::SteadyClock::now()), _name(name)
		{
			_end_timestamp = _start_timestamp;
		}

		virtual void Restart()
		{
			_start_timestamp = time::SteadyClock::now();
			_end_timestamp = _start_timestamp;
		}

		virtual void Stop()
		{
			_end_timestamp = time::SteadyClock::now();
		}

		time::SteadyTimestamp GetStartTimestamp()
		{
			return _start_timestamp;
		}

		time::SteadyTimestamp GetEndTimestamp()
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

		time::DurationMilliseconds GetElapsedMilliseconds()
		{
			time::DurationMilliseconds start(_start_timestamp.time_since_epoch());
			time::DurationMilliseconds end(_end_timestamp.time_since_epoch());
			return end - start;
		}

		time::DurationMicroseconds GetElapsedMicroseconds()
		{
			time::DurationMicroseconds start(_start_timestamp.time_since_epoch());
			time::DurationMicroseconds end(_end_timestamp.time_since_epoch());
			return end - start;
		}
	};
} // namespace np::insight

#endif /* NP_ENGINE_TIMER_HPP */
