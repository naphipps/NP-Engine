//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/28/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MPMC_QUEUE_HPP
#define NP_ENGINE_MPMC_QUEUE_HPP

#include <concurrentqueue.h>

#include "NP-Engine/Memory/Memory.hpp"

namespace np::concurrency
{
	namespace __detail
	{
		struct MpmcQueueTraits : public ::moodycamel::ConcurrentQueueDefaultTraits
		{
			static inline void* malloc(siz size)
			{
				return ::np::memory::TraitAllocator::malloc(size);
			}

			static inline void free(void* ptr)
			{
				::np::memory::TraitAllocator::free(ptr);
			}
		};
	} // namespace __detail

	template <class T>
	using MpmcQueue = ::moodycamel::ConcurrentQueue<T, __detail::MpmcQueueTraits>;
} // namespace np::concurrency

#endif /* NP_ENGINE_MPMC_QUEUE_HPP */
