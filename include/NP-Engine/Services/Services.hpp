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
#include "NP-Engine/Uid/Uid.hpp"

#include "NP-Engine/Vendor/EnttInclude.hpp"

namespace np::srvc
{
	class Services
	{
	private:
		mem::TraitAllocator _allocator;
		::entt::registry _ecs_registry;
		jsys::JobSystem _job_system;
		evnt::EventQueue _event_queue;
		evnt::EventSubmitter _event_submitter;
		uid::UidSystem _uid_system;

	public:
		Services(): _allocator(), _ecs_registry(), _job_system(), _event_queue(), _event_submitter(_event_queue) {}

		mem::Allocator& GetAllocator()
		{
			return _allocator;
		}

		const mem::Allocator& GetAllocator() const
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

		jsys::JobSystem& GetJobSystem()
		{
			return _job_system;
		}

		const jsys::JobSystem& GetJobSystem() const
		{
			return _job_system;
		}

		evnt::EventQueue& GetEventQueue()
		{
			return _event_queue;
		}

		const evnt::EventQueue& GetEventQueue() const
		{
			return _event_queue;
		}

		evnt::EventSubmitter& GetEventSubmitter()
		{
			return _event_submitter;
		}

		const evnt::EventSubmitter& GetEventSubmitter() const
		{
			return _event_submitter;
		}

		uid::UidSystem& GetUidSystem()
		{
			return _uid_system;
		}

		const uid::UidSystem& GetUidSystem() const
		{
			return _uid_system;
		}
	};
} // namespace np::srvc

#endif /* NP_ENGINE_SERVICES_HPP */