//
//  mpmc_queue.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/28/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_MPMC_QUEUE_HPP
#define NP_ENGINE_MPMC_QUEUE_HPP

#include <concurrentqueue.h>

#include "NP-Engine/Memory/Memory.hpp"

//TODO: add summary comments

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
    }

    template <class T>
    using MpmcQueue = ::moodycamel::ConcurrentQueue<T, __detail::MpmcQueueTraits>;
}

#endif /* NP_ENGINE_MPMC_QUEUE_HPP */
