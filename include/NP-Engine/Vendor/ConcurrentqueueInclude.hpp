//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/18/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_VENDOR_CONCURRENTQUEUE_INCLUDE_HPP
#define NP_ENGINE_VENDOR_CONCURRENTQUEUE_INCLUDE_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include <concurrentqueue.h>

namespace np::container
{
	namespace __detail
	{
		struct mpmc_queue_traits : public ::moodycamel::ConcurrentQueueDefaultTraits
		{
			static inline void* malloc(siz size)
			{
				return ::np::mem::TraitAllocator::malloc(size);
			}

			static inline void free(void* ptr)
			{
				::np::mem::TraitAllocator::free(ptr);
			}
		};
	} // namespace __detail

	template <class T>
	using mpmc_queue = ::moodycamel::ConcurrentQueue<T, __detail::mpmc_queue_traits>;
} // namespace np::container

#endif /* NP_ENGINE_VENDOR_CONCURRENTQUEUE_INCLUDE_HPP */