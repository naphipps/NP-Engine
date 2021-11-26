//
//  JobWorker.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/26/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_JOB_WORKER_HPP
#define NP_ENGINE_JOB_WORKER_HPP

#include <utility>

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"

#include "JobRecord.hpp"
#include "JobPool.hpp"
#include "JobToken.hpp"

namespace np
{
	namespace js
	{
		/**
		 Job worker will execute jobs
		 */
		class JobWorker
		{
		private:
			friend class JobSystem;

		public:
			using JobRecordQueue = concurrency::MpmcQueue<JobRecord>;

		private:
			static container::array<JobWorker*, concurrency::ThreadPool::MAX_THREAD_COUNT>
				_other_workers; // TODO: I don't really like this. Make it so this array can be local to the system instead of
								// global
			// container::vector<JobWorker*>* _neighbor_workers;//TODO: ^ use this vector pointer to add/remove neighbors, and
			// then steal from them
			static ui64 _other_workers_size;

			ui32 _id = 0;

			atm_bl _keep_working;
			atm_bl _work_procedure_incomplete;
			concurrency::ThreadToken _thread_token;
			concurrency::ThreadPool* _thread_pool;

			JobPool* _job_pool;

			constexpr static ui32 JOB_DEQUEUE_SIZE = 1024;
			JobRecordQueue _highest_job_deque;
			JobRecordQueue _higher_job_deque;
			JobRecordQueue _normal_job_deque;
			JobRecordQueue _lower_job_deque;
			JobRecordQueue _lowest_job_deque;

			ui32 _steal_counter;
			ui32 _steal_threshold;
			ui64 _other_worker_index;
			bl _refresh_other_worker_index;

			/**
			 gets the Job record deque for the given priority
			 */
			JobRecordQueue& GetQueueFromPriority(JobPriority priority)
			{
				JobRecordQueue* deque = nullptr;

				switch (priority)
				{
				case JobPriority::HIGHEST:
					deque = &_highest_job_deque;
					break;

				case JobPriority::HIGHER:
					deque = &_higher_job_deque;
					break;

				case JobPriority::NORMAL:
					deque = &_normal_job_deque;
					break;

				case JobPriority::LOWER:
					deque = &_lower_job_deque;
					break;

				case JobPriority::LOWEST:
					deque = &_lowest_job_deque;
					break;

				default:
					NP_ASSERT(false, "requested incorrect priority");
					break;
				}

				return *deque;
			}

			/**
			 gets the next Job for the worker to execute
			 */
			JobRecord GetNextJob()
			{
				JobRecord next_job;

				if (_highest_job_deque.try_dequeue(next_job)) {}
				else if (_higher_job_deque.try_dequeue(next_job))
				{}
				else if (_normal_job_deque.try_dequeue(next_job))
				{}
				else if (_lower_job_deque.try_dequeue(next_job))
				{}
				else if (_lowest_job_deque.try_dequeue(next_job))
				{}
				else
				{
					next_job.Invalidate();
				}

				return next_job;
			}

			/**
			 steals the next Job from another Job worker
			 */
			JobRecord GetStolenJob()
			{
				NP_PROFILE_FUNCTION();

				JobRecord stolen_job;

				if (_other_workers[0] != nullptr && _other_workers_size > 0)
				{
					if (_refresh_other_worker_index)
					{
						do
						{
							_other_worker_index++;
							_other_worker_index %= _other_workers_size;
						}
						while (this == _other_workers[_other_worker_index]);

						_refresh_other_worker_index = false;
					}

					if (_other_workers[_other_worker_index])
					{
						stolen_job = _other_workers[_other_worker_index]->GetNextJob();
					}
				}

				return stolen_job;
			}

			/**
			 the procedure for this Job worker to execute on a thread
			 */
			void WorkerThreadProcedure()
			{
				NP_PROFILE_SCOPE("WorkerThreadProcedure: " + to_str(_id));

				_work_procedure_incomplete.store(true, mo_release);

				while (_keep_working.load(mo_acquire))
				{
					JobRecord record = GetNextJob();
					if (record.IsValid() && record.GetJob().CanExecute())
					{
						NP_PROFILE_SCOPE("executing my Job");
						_steal_counter = 0;
						record.Execute();
						_job_pool->DestroyObject(const_cast<Job*>(&record.GetJob()));
					}
					else
					{
						if (record.IsValid() && record.GetJob().IsEnabled())
						{
							AddJob(NormalizePriority(record.GetPriority()), &record.GetJob());
						}

						if (_steal_counter > _steal_threshold)
						{
							JobRecord stolen = GetStolenJob();
							if (stolen.IsValid() && stolen.GetJob().CanExecute())
							{
								NP_PROFILE_SCOPE("executing stolen Job");
								stolen.Execute();
								_job_pool->DestroyObject(const_cast<Job*>(&stolen.GetJob()));
							}
							else
							{
								_steal_counter = 0;

								if (stolen.IsValid() && stolen.GetJob().IsEnabled())
								{
									AddJob(NormalizePriority(stolen.GetPriority()), &stolen.GetJob());
								}
								else
								{
									// we did not steal a good job thus we want to steal from someone else
									_refresh_other_worker_index = true;
								}
							}
						}
						else
						{
							_steal_counter++;
							concurrency::ThisThread::yield();
						}
					}
				}

				_work_procedure_incomplete.store(false, mo_release);
			}

			/**
			 creates Job from pool given function and parent
			 */
			Job* CreateJob(JobFunction& function, Job* dependent = nullptr)
			{
				// TODO: clean up this api
				//                return _job_pool->CreateJob(function, dependent);
				Job* job = _job_pool->CreateObject();

				if (dependent)
				{
					job->Init(function, *dependent);
				}
				else
				{
					job->Init(function);
				}

				return job;
			}

			/**
			 adds Job given the priority, and Job pointer
			 */
			JobRecord AddJob(JobPriority priority, const Job* job)
			{
				return AddJob(JobRecord(priority, job));
			}

			/**
			 addes Job given the Job record
			 */
			JobRecord AddJob(JobRecord record)
			{
				NP_ASSERT(record.IsValid(), "attempted to add an invalid Job -- do not do that my guy");
				NP_ASSERT(record.GetJob().IsEnabled(), "the dude not enabled bro - why it do");

				JobRecord return_record;

				if (GetQueueFromPriority(record.GetPriority()).enqueue(record))
				{
					return_record = record;
				}

				return return_record;
			}

		public:
			/**
			 constructor
			 */
			JobWorker():
				_highest_job_deque(JOB_DEQUEUE_SIZE),
				_higher_job_deque(JOB_DEQUEUE_SIZE),
				_normal_job_deque(JOB_DEQUEUE_SIZE),
				_lower_job_deque(JOB_DEQUEUE_SIZE),
				_lowest_job_deque(JOB_DEQUEUE_SIZE),
				_keep_working(false),
				_work_procedure_incomplete(false),
				_thread_pool(nullptr),
				_steal_counter(0),
				_steal_threshold(0),
				_other_worker_index(0),
				_refresh_other_worker_index(true)
			{
				for (ui32 i = 0; i < _other_workers.size(); i++)
				{
					_other_workers[i] = nullptr;
				}
			}

			/**
			 deconstructor
			 */
			~JobWorker()
			{
				StopWork();
			}

			/**
			 creates Job from pool given function and parent
			 */
			JobToken CreateJobToken(JobFunction& function, JobToken dependent = JobToken())
			{
				JobToken token(CreateJob(function));

				if (token.IsValid() && dependent.IsValid())
				{
					dependent.GetJob().DependOn(token.GetJob());
				}

				return token;
			}

			/**
			 adds job given the priority and the job token from CreateJobToken
			 */
			JobRecord AddJob(JobPriority priority, JobToken token)
			{
				JobRecord record;

				if (token.IsValid())
				{
					record = AddJob(priority, &token.GetJob());
				}

				return record;
			}

			/**
			 adds job given the priority, function, and optional dependent
			 */
			JobRecord AddJob(JobPriority priority, JobFunction& function, JobToken dependent = JobToken())
			{
				return AddJob(priority, CreateJobToken(function, dependent));
			}

			/**
			 sets this Job worker to start work given the thread pool (to create a thread to work on), and thread affinity
			 a thread affinity of -1 disables thread affinity, else values should be [0, hardware_concurrency)
			 */
			void StartWork(concurrency::ThreadPool& pool, i32 thread_affinity = -1)
			{
				NP_PROFILE_FUNCTION();
				_keep_working.store(true, mo_release);
				_thread_pool = &pool;
				_thread_token = _thread_pool->CreateThread(thread_affinity, &JobWorker::WorkerThreadProcedure, this);
			}

			/**
			 sets the Job worker to stop work
			 */
			void StopWork()
			{
				NP_PROFILE_FUNCTION();
				_keep_working.store(false, mo_release);

				do
				{
					concurrency::ThisThread::yield();
				}
				while (_work_procedure_incomplete);

				if (_thread_pool && _thread_token.IsValid())
				{
					_thread_pool->RemoveThread(_thread_token);
					_thread_token.Invalidate();
					_thread_pool = nullptr;
				}
			}
		};
	} // namespace js
} // namespace np

#endif /* NP_ENGINE_JOB_WORKER_HPP */
