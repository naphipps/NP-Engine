//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_APPLICATION_CLOSE_EVENT_HPP
#define NP_ENGINE_APPLICATION_CLOSE_EVENT_HPP

#include "NP-Engine/Event/Event.hpp"

namespace np::app
{
	class ApplicationCloseEvent : public evnt::Event
	{
	public:
		ApplicationCloseEvent(): evnt::Event() {}

		virtual evnt::EventType GetType() const override
		{
			return evnt::EventType::ApplicationClose;
		}

		virtual evnt::EventCategory GetCategory() const override
		{
			return evnt::EventCategory::Application;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_APPLICATION_CLOSE_EVENT_HPP */
