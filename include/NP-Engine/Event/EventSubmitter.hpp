//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/16/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_EGNINE_EVENT_SUBMITTER_HPP
#define NP_EGNINE_EVENT_SUBMITTER_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "EventQueue.hpp"
#include "EventImpl.hpp"

namespace np::event
{
	class EventSubmitter
	{
	private:
		EventQueue& _queue;

	public:
		EventSubmitter(EventQueue& queue): _queue(queue) {}

		template <typename T, typename... Args>
		bl Emplace(Args&&... args)
		{
			return _queue.Emplace<T>(::std::forward<Args>(args)...);
		}

		bl Emplace(const Event* e)
		{
			return _queue.Emplace(e);
		}
	};
} // namespace np::event

#endif /* NP_EGNINE_EVENT_SUBMITTER_HPP */