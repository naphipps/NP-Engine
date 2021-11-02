//
//  JobSystemImpl.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/25/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JOB_SYSTEM_IMPL_HPP
#define NP_ENGINE_JOB_SYSTEM_IMPL_HPP

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "JobWorker.hpp"
#include "JobWorkerToken.hpp"
#include "JobPool.hpp"

namespace np
{
    namespace js
    {
        //TODO: support our allocators here
        
        /**
         the cornerstone object for the job system
         */
        class JobSystem
        {
        private:
            container::array<JobWorker, concurrency::ThreadPool::MAX_THREAD_COUNT> _job_worker;
            siz _job_worker_count;
            concurrency::ThreadPool _thread_pool;
            JobPool _job_pool;
            bl _running;
            
        public:
            
            /**
             constructor
             */
            JobSystem():
            _job_pool(2000),
            _running(false)
            {
                _job_worker_count = _thread_pool.ObjectCount() - 1 < _job_worker.size() ? _thread_pool.ObjectCount() - 1 : _job_worker.size();
                
                //setup other_workers for job workers to reference the other workers for job stealing
                for (ui32 i=0; i<JobWorker::_other_workers.size(); i++)
                {
                    JobWorker::_other_workers[i] = &_job_worker[i];
                    JobWorker::_other_workers[i]->_id = i;
                    JobWorker::_other_workers[i]->_job_pool = &_job_pool;
                    JobWorker::_other_workers[i]->_other_worker_index = (i + 1) % _job_worker_count;
                    JobWorker::_other_workers[i]->_refresh_other_worker_index = false;
                }
                
                JobWorker::_other_workers_size = _job_worker_count;
            }
            
            /**
             deconstructor //TODO: refactor all "deconstruct" to "destruct"
             */
            ~JobSystem()
            {
                if (IsRunning())
                {
                    Stop();
                }
            }
            
            /**
             starts all the workers of the job system
             */
            void Start()
            {
                NP_PROFILE_FUNCTION();
                
                _running = true;
                
                for (siz i=0; i<_job_worker_count; i++)
                {
                    _job_worker[i].StartWork(_thread_pool, (i + 1) % _thread_pool.ObjectCount());
                }
            }
            
            /**
             stops all the workers of the job system
             */
            void Stop()
            {
                NP_PROFILE_FUNCTION();
                
                if (IsRunning())
                {
                    //stop workers
                    for (ui64 i=0; i<_job_worker_count; i++)
                    {
                        _job_worker[i].StopWork();
                    }
                    
                    //stop threads
                    _thread_pool.Clear();
                    _running = false;
                }
            }
            
            /**
             indicates if the job system is running or not
             */
            bl IsRunning()
            {
                return _running;
            }
            
            /**
             gets the count of job workers in the job system
             */
            ui64 GetJobWorkerCount()
            {
                return _job_worker_count;
            }
            
            /**
             gets the job worker token given the index
             index [0, GetJobWorkerCount)
             */
            JobWorkerToken GetJobWorker(ui64 index)
            {
                return JobWorkerToken(&_job_worker[index]);
            }
        };
    }
}

#endif /* NP_ENGINE_JOB_SYSTEM_IMPL_HPP */
