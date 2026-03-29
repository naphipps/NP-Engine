//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_NSIT_SCOPED_TIMER_HPP
#define NP_ENGINE_NSIT_SCOPED_TIMER_HPP

#include <string>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Timer.hpp"

namespace np::nsit
{
	class scoped_timer : public timer
	{
	public:
		using action = void (*)(scoped_timer&);

	private:
		action _stopped_action;
		bl _stopped;

	public:
		scoped_timer(action a = nullptr): scoped_timer("", a) {}

		scoped_timer(::std::string name, action a = nullptr): timer(name), _stopped_action(a), _stopped(false) {}

		virtual ~scoped_timer()
		{
			if (!_stopped)
				stop();
		}

		void set_stopped_action(action a)
		{
			_stopped_action = a;
		}

		void stop() override
		{
			if (!_stopped)
			{
				timer::stop();
				_stopped = true;

				if (_stopped_action)
					_stopped_action(*this);
			}
		}

		void restart() override
		{
			timer::restart();
			_stopped = false;
		}
	};
} // namespace np::nsit

#endif /* NP_ENGINE_NSIT_SCOPED_TIMER_HPP */
