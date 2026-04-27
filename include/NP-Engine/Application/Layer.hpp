//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/9/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_LAYER_HPP
#define NP_ENGINE_LAYER_HPP

#include "NP-Engine/Time/Time.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/JobSystem/JobSystem.hpp"
#include "NP-Engine/Services/Services.hpp"

namespace np::app
{
	/*
		Layer will represent a layer in our application
	*/
	class Layer : public evnt::EventHandler
	{
	protected:
		mem::sptr<srvc::Services> _services;

		virtual void HandleEvent(mem::sptr<evnt::Event> e) override {}

	public:
		Layer(mem::sptr<srvc::Services> services): _services(services) {}

		virtual void BeforePoll() {}

		/*
			Polling is for interaction with human interfaces.
			Depending on the platform, this could be blocking.
			It is recommended to always keep polling on the main thread.
		*/
		virtual void Poll(tim::milliseconds time_delta) {}

		virtual void AfterPoll() {}

		virtual void CleanupPoll() {}

		virtual void BeforeUpdate() {}

		/*
			This is the common application loop update.
			If polling is blocking, it could be useful to move this outside the main thread.
		*/
		virtual void Update(tim::milliseconds time_delta) {}

		virtual void AfterUpdate() {}

		virtual void CleanupUpdate() {}

		virtual bl CanHandle(evnt::EventType type) const override
		{
			return false;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_LAYER_HPP */