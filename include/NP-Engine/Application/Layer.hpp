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

		virtual void BeforeUdpate() {}

		virtual void Update(tim::DblMilliseconds time_delta) {}

		virtual void AfterUdpate() {}

		virtual void Cleanup() {}

		virtual evnt::EventCategory GetHandledCategories() const override
		{
			return evnt::EventCategory::None;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_LAYER_HPP */