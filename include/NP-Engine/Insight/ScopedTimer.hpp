//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 5/19/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SCOPED_TIMER_HPP
#define NP_ENGINE_SCOPED_TIMER_HPP

#include <string>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Timer.hpp"

namespace np::nsit
{
	class ScopedTimer : public Timer
	{
	public:
		using Action = void (*)(ScopedTimer&);

	private:
		Action _stopped_action;
		bl _stopped;

	public:
		ScopedTimer(Action action = nullptr): ScopedTimer("", action) {}

		ScopedTimer(::std::string name, Action action = nullptr): Timer(name), _stopped_action(action), _stopped(false) {}

		virtual ~ScopedTimer()
		{
			if (!_stopped)
			{
				Stop();
			}
		}

		void SetStoppedAction(Action action)
		{
			_stopped_action = action;
		}

		void Stop() override
		{
			if (!_stopped)
			{
				Timer::Stop();
				_stopped = true;

				if (_stopped_action)
				{
					_stopped_action(*this);
				}
			}
		}

		void Restart() override
		{
			Timer::Restart();
			_stopped = false;
		}
	};
} // namespace np::nsit

#endif /* NP_ENGINE_SCOPED_TIMER_HPP */
