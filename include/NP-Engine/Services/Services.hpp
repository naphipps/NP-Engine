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
#include "NP-Engine/Input/Input.hpp"

namespace np::srvc
{
	class Services
	{
	private:
		mem::TraitAllocator _allocator;
		jsys::JobSystem _job_system;
		evnt::EventQueue _event_queue;
		evnt::EventSubmitter _event_submitter; // TODO: I do not think we need this
		uid::UidSystem _uid_system;
		nput::InputQueue _input_queue;

	public:
		Services(): _allocator(), _job_system(), _event_queue(), _event_submitter(_event_queue) {}

		mem::Allocator& GetAllocator()
		{
			return _allocator;
		}

		const mem::Allocator& GetAllocator() const
		{
			return _allocator;
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

		nput::InputQueue& GetInputQueue()
		{
			return _input_queue;
		}

		const nput::InputQueue& GetInputQueue() const
		{
			return _input_queue;
		}
	};
} // namespace np::srvc

#endif /* NP_ENGINE_SERVICES_HPP */