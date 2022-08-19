//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/17/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SERVICES_HPP
#define NP_ENGINE_SERVICES_HPP

#include "NP-Engine/JobSystem/JobSystem.hpp"
#include "NP-Engine/Event/Event.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

namespace np::services
{
	//TODO: we need to create this before we create our application and then destroy it after our application, inside our application allocator

	class Services
	{
	private:
		memory::TraitAllocator _allocator;
		::entt::registry _ecs_registry;
		js::JobSystem _job_system;
		event::EventQueue _event_queue;
		event::EventSubmitter _event_submitter;
		
	public:
		Services() :
			_allocator(),
			_ecs_registry(),
			_job_system(),
			_event_queue(),
			_event_submitter(_event_queue)
		{}

		memory::Allocator& GetAllocator()
		{
			return _allocator;
		}

		const memory::Allocator& GetAllocator() const
		{
			return _allocator;
		}

		::entt::registry& GetEcsRegistry()
		{
			return _ecs_registry;
		}

		const ::entt::registry& GetEcsRegistry() const
		{
			return _ecs_registry;
		}

		js::JobSystem& GetJobSystem()
		{
			return _job_system;
		}

		const js::JobSystem& GetJobSystem() const
		{
			return _job_system;
		}

		event::EventQueue& GetEventQueue()
		{
			return _event_queue;
		}

		const event::EventQueue& GetEventQueue() const
		{
			return _event_queue;
		}

		event::EventSubmitter& GetEventSubmitter()
		{
			return _event_submitter;
		}

		const event::EventSubmitter& GetEventSubmitter() const
		{
			return _event_submitter;
		}
	};
}

#endif /* NP_ENGINE_SERVICES_HPP */