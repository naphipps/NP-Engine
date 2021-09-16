//
//  queue.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/21/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_QUEUE_HPP
#define NP_ENGINE_QUEUE_HPP

#include <queue>

namespace np
{
    namespace container
    {
        /**
         brings the std queue here for it's usefulness
         */
        template<typename T>
        using queue = ::std::queue<T>;
    }
}

#endif /* NP_ENGINE_QUEUE_HPP */
