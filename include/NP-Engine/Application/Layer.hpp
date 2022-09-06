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
		srvc::Services& _services;

		virtual void HandleEvent(evnt::Event& e) {}

	public:
		Layer(srvc::Services& services): _services(services) {}

		virtual void BeforeUdpate() {} // TODO: I think we'll decide if we like this idea if it works with imgui

		virtual void Update(tim::DblMilliseconds time_delta) {}

		virtual void AfterUdpate() {}

		virtual void Cleanup() {}

		virtual evnt::EventCategory GetHandledCategories() const
		{
			return evnt::EventCategory::None;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_LAYER_HPP */