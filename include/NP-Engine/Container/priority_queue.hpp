//
//  priority_queue.hpp
//  NP-Engine
//
//  Created by Nathan Phipps on 7/25/21.
//

#ifndef NP_ENGINE_PRIORITY_QUEUE_HPP
#define NP_ENGINE_PRIORITY_QUEUE_HPP

#include <queue>

#include "NP-Engine/vector.hpp"

namespace np::container
{
    template <
    class T,
    class Container = vector<T>, //TODO: how do we determine allocator?! maybe we need to inherit from std priority queue?
    class Compare = ::std::less<typename Container::value_type>
    >
    using pqueue = ::std::priority_queue<T, Container, Compare>;
}

#endif /* NP_ENGINE_PRIORITY_QUEUE_HPP */
