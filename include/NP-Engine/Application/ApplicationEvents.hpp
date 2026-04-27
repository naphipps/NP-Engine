//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/27/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_APP_APPLICATION_EVENTS_HPP
#define NP_ENGINE_APP_APPLICATION_EVENTS_HPP

#include "NP-Engine/Event/Event.hpp"

namespace np::app
{
	class ApplicationCloseEvent : public evnt::Event
	{
	protected:
		evnt::EventType _type;

	public:
		ApplicationCloseEvent(evnt::EventType intention):
			evnt::Event(),
			_type(evnt::EventType::Application | evnt::EventType::Close | intention.GetIntention())
		{}

		virtual evnt::EventType GetEventType() const override
		{
			return _type;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_APP_APPLICATION_EVENTS_HPP */
