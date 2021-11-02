//
//  JobPool.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/26/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JOB_POOL_HPP
#define NP_ENGINE_JOB_POOL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

#include "Job.hpp"

namespace np
{
    namespace js
    {
        using JobPool = memory::ObjectPool<Job>;
    }
}

#endif /* NP_ENGINE_JOB_POOL_HPP */
