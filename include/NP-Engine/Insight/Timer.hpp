//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NSIT_TIMER_HPP
#define NP_ENGINE_NSIT_TIMER_HPP

#include <string>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Time/Time.hpp"

namespace np::nsit
{
	class timer
	{
	protected:
		::std::string _name;
		tim::steady_timestamp _start_timestamp;
		tim::steady_timestamp _end_timestamp;

	public:
		timer(::std::string name = ""): _start_timestamp(tim::steady_clock::now()), _name(name)
		{
			_end_timestamp = _start_timestamp;
		}

		virtual void restart()
		{
			_start_timestamp = tim::steady_clock::now();
			_end_timestamp = _start_timestamp;
		}

		virtual void stop()
		{
			_end_timestamp = tim::steady_clock::now();
		}

		tim::steady_timestamp get_start_timestamp()
		{
			return _start_timestamp;
		}

		tim::steady_timestamp get_end_timestamp()
		{
			return _end_timestamp;
		}

		void set_name(::std::string name)
		{
			_name = name;
		}

		::std::string get_name()
		{
			return _name;
		}

		tim::milliseconds get_elapsed_milliseconds()
		{
			return _end_timestamp.time_since_epoch() - _start_timestamp.time_since_epoch();
		}

		tim::milliseconds get_elapsed_microseconds()
		{
			return _end_timestamp.time_since_epoch() - _start_timestamp.time_since_epoch();
		}
	};
} // namespace np::nsit

#endif /* NP_ENGINE_NSIT_TIMER_HPP */
