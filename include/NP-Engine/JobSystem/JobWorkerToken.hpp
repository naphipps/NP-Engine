//
//  JobWOrkerToken.hpp
//  Project Space
//
//  Created by Nathan Phipps on 9/9/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JOB_WORKER_TOKEN_HPP
#define NP_ENGINE_JOB_WORKER_TOKEN_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "JobWorker.hpp"
#include "JobToken.hpp"

namespace np
{
    namespace js
    {
        /**
         contains a pointer to a Job worker so that outside objects cannot manipulate it
         */
        class JobWorkerToken
        {
        private:
            JobWorker* _worker;
            
        public:
            
            /**
             constructor
             */
            JobWorkerToken():
            JobWorkerToken(nullptr)
            {}
            
            /**
             constructor - set worker pointer to given pointer
             */
            JobWorkerToken(JobWorker* worker):
            _worker(worker)
            {}
            
            /**
             copy constructor
             */
            JobWorkerToken(JobWorkerToken& other):
            _worker(other._worker)
            {}
            
            /**
             move constructor -acts like copy constructor
             */
            JobWorkerToken(JobWorkerToken&& other):
            _worker(other._worker)
            {}
            
            /**
             deconstructor
             */
            ~JobWorkerToken()
            {}
            
            /**
             checks if our worker pointer is nullptr or not
             */
            bl IsValid() const
            {
                return _worker != nullptr;
            }
            
            /**
             invalidate worker pointer by setting it to nullptr
             */
            void Invalidate()
            {
                _worker = nullptr;
            }
                        
            /**
             calls JobWorker::CreateJob on our worker pointer
             */
            JobToken CreateJobToken(JobFunction& function, JobToken dependent = JobToken())
            {
                JobToken token;
                
                if (IsValid())
                {
                    token = _worker->CreateJobToken(function, dependent);
                }
                
                return token;
            }
            
            /**
             calls JobWorker::AddJob on our worker pointer
             */
            JobRecord AddJob(JobPriority priority, JobToken token)
            {
                JobRecord record;
                
                if (IsValid())
                {
                    record = _worker->AddJob(priority, token);
                }
                
                return record;
            }
            
            /**
             calls JobWorker::AddJob on our worker pointer
             */
            JobRecord AddJob(JobPriority priority, JobFunction function, JobToken dependent = JobToken())
            {
                JobRecord record;
                
                if (IsValid())
                {
                    record = _worker->AddJob(priority, function, dependent);
                }
                
                return record;
            }
            
            /**
             indicates if our token is valid or not
             */
            explicit operator bl() const
            {
                return IsValid();
            }
            
            /**
             copy assignment
             */
            JobWorkerToken& operator=(const JobWorkerToken& other)
            {
                _worker = other._worker;
                return *this;
            }
            
            /**
             move assignment - acts as copy assignment
             */
            JobWorkerToken& operator=(JobWorkerToken&& other)
            {
                _worker = other._worker;
                return *this;
            }
        };
    }
}

#endif /* NP_ENGINE_JOB_WORKER_TOKEN_HPP */
