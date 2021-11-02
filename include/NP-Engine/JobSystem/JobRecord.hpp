//
//  JobRecord.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/31/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JOB_RECORD_HPP
#define NP_ENGINE_JOB_RECORD_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Job.hpp"
#include "JobPriority.hpp"

namespace np
{
    namespace js
    {
        /**
         job record associtates a job to a priority
         */
        class JobRecord
        {
        private:
            JobPriority _priority;
            Job* _job;
            
        public:
            
            /**
             constuctor - normal priority with NULL job
             */
            JobRecord():
            JobRecord(JobPriority::NORMAL, NULL)
            {}

            /**
             constructor - accepts given priority and job
             */
            JobRecord(JobPriority priority, const Job* job):
            _priority(priority),
            _job(const_cast<Job*>(job))
            {}
            
            /**
             copy constructor
             */
            JobRecord(const JobRecord& record):
            JobRecord(record._priority, record._job)
            {}
                        
            /**
             move constructor - behave like copy constructor
             */
            JobRecord(JobRecord&& record):
            JobRecord(record._priority, record._job)
            {}
            
            /**
             checks if our job is NULL or not
             */
            bl IsValid() const
            {
                return _job != NULL;
            }
            
            /**
             invalidates our job by setting it to NULL
             */
            void Invalidate()
            {
                _job = NULL;
            }
            
            /**
             gets the job this record pertains to
             this will assert that the job is valid so you should check if the record is valid before calling this
             */
            const Job& GetJob() const
            {
                NP_ASSERT(IsValid(), "do not call this if we have an invalid job");
                
                return *_job;
            }
            
            /**
             executes the job
             */
            void Execute()
            {
                NP_ASSERT(IsValid(), "do not call this if we have an invalid job");
                
                _job->Execute();
            }
            
            /**
             gets the priority of this record
             */
            JobPriority GetPriority() const
            {
                return _priority;
            }
            
            /**
             bl operator checks if we have a NULL job or not
             */
            explicit operator bl() const
            {
                return IsValid();
            }
            
            /**
             copy assignment
             */
            JobRecord& operator=(const JobRecord& other)
            {
                _priority = other._priority;
                _job = other._job;
                return *this;
            }

            /**
             move assignment is treated as copy assignment
             */
            JobRecord& operator=(JobRecord&& other)
            {
                _priority = other._priority;
                _job = other._job;
                return *this;
            }
        };
    }
}

#endif /* NP_ENGINE_JOB_RECORD_HPP */
