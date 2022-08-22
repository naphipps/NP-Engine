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
	class Layer : public event::EventHandler
	{
	protected:
		services::Services& _services;

		virtual void HandleEvent(event::Event& e) {}

	public:
		Layer(services::Services& services): _services(services) {}

		virtual void BeforeUdpate() {} // TODO: I think we'll decide if we like this idea if it works with imgui

		virtual void Update(tim::DurationMilliseconds time_delta) {}

		virtual void AfterUdpate() {}

		virtual void Cleanup() {}

		virtual event::EventCategory GetHandledCategories() const
		{
			return event::EventCategory::None;
		}
	};
} // namespace np::app

#endif /* NP_ENGINE_LAYER_HPP */