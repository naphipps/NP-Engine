//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/13/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_EVENT_HANDLER_HPP
#define NP_ENGINE_EVENT_HANDLER_HPP

#include "EventImpl.hpp"
#include "EventSubmitter.hpp"

namespace np::evnt
{
	class EventHandler
	{
	protected:
		virtual void HandleEvent(mem::sptr<Event> e) = 0;

	public:
		virtual void OnEvent(mem::sptr<Event> e)
		{
			if (CanHandle(e->GetEventType()))
				HandleEvent(e);
		}

		virtual bl CanHandle(EventType type) const = 0;
	};
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_HANDLER_HPP */