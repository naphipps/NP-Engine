//
//  JobToken.hpp
//  Project Space
//
//  Created by Nathan Phipps on 1/6/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JOB_TOKEN_HPP
#define NP_ENGINE_JOB_TOKEN_HPP

#include "Primitive/Primitive.hpp"
#include "Insight/Insight.hpp"

#include "Job.hpp"

namespace np
{
    namespace js
    {
        class JobToken
        {
        private:
            Job* _job;
            
        public:
            
            /**
             constructor
             */
            JobToken(Job* j = NULL):
            _job(j)
            {}
            
            /**
             bl operator
             */
            inline explicit operator bl() const
            {
                return IsValid();
            }
            
            /**
             checks if we are valid by checking if our inner job pointer is null or not
             */
            inline bl IsValid() const
            {
                return _job != NULL;
            }
            
            /**
             gets the job associated with this token
             */
            inline Job& GetJob()
            {
                NP_ASSERT(IsValid(), "we require a valid token when getting the job");
                return *_job;
            }
            
            /**
             gets the job associated with this token
             */
            inline const Job& GetJob() const
            {
                NP_ASSERT(IsValid(), "we require a valid token when getting the job");
                return *_job;
            }
        };
    }
}

#endif /* NP_ENGINE_JOB_TOKEN_HPP */
