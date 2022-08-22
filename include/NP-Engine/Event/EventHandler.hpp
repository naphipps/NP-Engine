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
		virtual void HandleEvent(Event& e) = 0;

	public:
		virtual void OnEvent(Event& e)
		{
			if (EventCategoryContains(GetHandledCategories(), e.GetCategory()))
			{
				HandleEvent(e);
			}
		}

		virtual EventCategory GetHandledCategories() const = 0;
	};
} // namespace np::evnt

#endif /* NP_ENGINE_EVENT_HANDLER_HPP */