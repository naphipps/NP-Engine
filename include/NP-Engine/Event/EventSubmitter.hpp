//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/16/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_EGNINE_EVENT_SUBMITTER_HPP
#define NP_EGNINE_EVENT_SUBMITTER_HPP

#include <utility>

#include "NP-Engine/Primitive/Primitive.hpp"

#include "EventQueue.hpp"
#include "EventImpl.hpp"

namespace np::evnt
{
	class EventSubmitter
	{
	private:
		EventQueue& _queue;

	public:
		EventSubmitter(EventQueue& queue): _queue(queue) {}

		void Submit(mem::sptr<Event> e)
		{
			_queue.Push(e);
		}
	};
} // namespace np::evnt

#endif /* NP_EGNINE_EVENT_SUBMITTER_HPP */