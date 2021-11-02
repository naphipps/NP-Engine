//
//  JobPriority.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/31/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JOB_PRIORITY_HPP
#define NP_ENGINE_JOB_PRIORITY_HPP

namespace np
{
    namespace js
    {
        /**
         job priority enum provides priority levels for jobs in the system
         */
        enum class JobPriority
        {
            HIGHEST,
            HIGHER,
            NORMAL,
            LOWER,
            LOWEST
        };
        
        /**
         "normalizes" the given priority - returns the immediate priority next in the direction of normal
         */
        static inline JobPriority NormalizePriority(JobPriority priority)
        {
            JobPriority return_priority = priority;
            
            switch (priority)
            {
                case JobPriority::HIGHEST:
                    return_priority = JobPriority::HIGHER;
                    break;
                    
                case JobPriority::HIGHER:
                    return_priority = JobPriority::NORMAL;
                    break;
                    
                case JobPriority::NORMAL:
                    return_priority = JobPriority::NORMAL;
                    break;
                    
                case JobPriority::LOWER:
                    return_priority = JobPriority::NORMAL;
                    break;
                    
                case JobPriority::LOWEST:
                    return_priority = JobPriority::LOWER;
                    break;
                    
                default:
                    NP_ASSERT(false, "requested incorrect priority");
                    break;
            }
            
            return return_priority;
        }
    }
}

#endif /* NP_ENGINE_JOB_PRIORITY_HPP */
