//
//  Job.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/25/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JOB_HPP
#define NP_ENGINE_JOB_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"

#include "JobFunction.hpp"

namespace np
{
    namespace js
    {
        /**
         represents a Job to be executred
         */
        class Job
        {
        private:
            Job* _dependent_job; //job depending on this
            atm_i32 _dependency_count;
            JobFunction _job_function;
            
            /**
             disposes this Job so it can be used again to represent another Job
             */
            void Dispose()
            {
                if (_dependent_job && !IsComplete())
                {
                    _dependent_job->_dependency_count--;
                }
                
                _dependent_job = nullptr;
                _job_function.Invalidate();
                _dependency_count = -1;
            }
            
            /**
             copies from the given other Job
             */
            inline void CopyFrom(const Job& other)
            {
                _dependency_count.store(other._dependency_count);

                if (other.IsEnabled())
                {
                    _dependent_job = other._dependent_job;
                    _job_function = other._job_function;
                }
            }
            
        public:
            
            /**
             constructor
             */
            Job():
            _dependent_job(nullptr),
            _dependency_count(-1)
            {}
            
            /**
             constructor - sets this Job's function to the given function
             */
            Job(JobFunction& function)
            {
                Init(function);
            }
            
            /**
             copy constructor
             */
            Job(Job& other)
            {
                CopyFrom(other);
            }
            
            /**
             copy assignment
             */
            Job& operator=(const Job& other)
            {
                if (_dependent_job && !IsComplete())
                {
                    _dependent_job->_dependency_count--;
                    _dependent_job = nullptr;
                }
                
                //TODO: investigate the following assert
                NP_ASSERT(CanExecute(), "You are overwriting a Job that has dependents. Only overwrite jobs that can execute.");
                
                CopyFrom(other);
                return *this;
            }
            
            /**
             delete move constructor
             */
            Job(Job&&) = delete;
            
            /**
             delete move assignment
             */
            Job& operator=(Job&&) = delete;
            
            /**
             deconstructor
             */
            ~Job()
            {
                Dispose();
            }
                 
            /**
             initializes this Job given the parent-Job, and function
             */
            void Init(JobFunction& function)
            {
                _dependent_job = nullptr;
                _job_function = function;
                _dependency_count = 1;
            }
            
            /**
             initializes this Job given the parent-Job, and function
             */
            void Init(JobFunction& function, Job& dependent)//TODO: should we remove this dependent param?? -- just seems like a weird param for init
            {
                Init(function);
                dependent.DependOn(*this);
            }
            
            /**
             indicates whether or not this Job can execute
             */
            bl CanExecute() const
            {
                return _dependency_count == 1;
            }
            
            /**
             executes this Job's function
             */
            void Execute()
            {
                if (CanExecute())
                {
                    _job_function(_job_function);
                    _dependency_count--;
                    
                    if (_dependent_job)
                    {
                        _dependent_job->_dependency_count--;
                    }
                }
            }
            
            /**
             indicates whether or not this Job is complete
             */
            bl IsComplete() const
            {
                return _dependency_count == 0;
            }
            
            /**
             indicates whether or not this Job is enabled
             */
            bl IsEnabled() const
            {
                return _dependency_count > -1;
            }
            
            /**
             indicates if this Job is enabled or not
             */
            inline explicit operator bl() const
            {
                return IsEnabled();
            }
            
            /**
             sets this Job dependent to the given Job
             */
            void DependOn(Job& job)
            {
                if (IsEnabled())
                {
                    if (job._dependent_job)
                    {
                        job._dependent_job->_dependency_count--;
                    }
                    
                    job._dependent_job = this;
                    _dependency_count++;
                }
            }
        };
    }
}

#endif /* NP_ENGINE_JOB_HPP */
