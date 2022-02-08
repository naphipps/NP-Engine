//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/26/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_WORKER_HPP
#define NP_ENGINE_JOB_WORKER_HPP

#include <utility>

#include "NP-Engine/Container/Container.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/String/String.hpp"
#include "NP-Engine/Random/Random.hpp"
#include "NP-Engine/Time/Time.hpp"

#include "JobRecord.hpp"
#include "JobPool.hpp"
#include "JobToken.hpp"

namespace np::js
{
	class JobWorker
	{
	private:
		friend class JobSystem;

	public:
		using JobRecordQueue = concurrency::MpmcQueue<JobRecord>;

	private:
		atm_bl _keep_working;
		atm_bl _work_procedure_complete;
		concurrency::ThreadToken _thread_token;
		concurrency::ThreadPool* _thread_pool;

		JobPool* _job_pool;

		constexpr static ui32 JOB_DEQUEUE_SIZE = 1024;
		JobRecordQueue _highest_job_deque;
		JobRecordQueue _higher_job_deque;
		JobRecordQueue _normal_job_deque;
		JobRecordQueue _lower_job_deque;
		JobRecordQueue _lowest_job_deque;

		container::vector<JobWorker*> _coworkers;
		ui32 _coworker_index;
		time::DurationMilliseconds _bad_steal_sleep_duration;
		random::Random32 _random_engine;

		JobRecordQueue& GetQueueFromPriority(JobPriority priority)
		{
			JobRecordQueue* deque = nullptr;

			switch (priority)
			{
			case JobPriority::Highest:
				deque = &_highest_job_deque;
				break;

			case JobPriority::Higher:
				deque = &_higher_job_deque;
				break;

			case JobPriority::Normal:
				deque = &_normal_job_deque;
				break;

			case JobPriority::Lower:
				deque = &_lower_job_deque;
				break;

			case JobPriority::Lowest:
				deque = &_lowest_job_deque;
				break;

			default:
				NP_ASSERT(false, "requested incorrect priority");
				break;
			}

			return *deque;
		}

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

		JobRecord GetStolenJob()
		{
			NP_PROFILE_FUNCTION();

			JobRecord stolen_job;

			if (!_coworkers.empty())
			{
				stolen_job = _coworkers[_coworker_index]->GetNextJob();
			}

			return stolen_job;
		}

		void WorkerThreadProcedure()
		{
			NP_PROFILE_SCOPE("WorkerThreadProcedure: " + to_str((ui64)this));

			while (_keep_working.load(mo_acquire))
			{
				JobRecord record = GetNextJob();
				if (record.IsValid() && record.GetJob().CanExecute())
				{
					NP_PROFILE_SCOPE("executing my Job");
					record.Execute();
					_job_pool->DestroyObject(const_cast<Job*>(&record.GetJob()));
				}
				else
				{
					if (record.IsValid() && record.GetJob().IsEnabled())
					{
						AddJob(NormalizePriority(record.GetPriority()), &record.GetJob());
					}

					JobRecord stolen = GetStolenJob();
					if (stolen.IsValid() && stolen.GetJob().CanExecute())
					{
						NP_PROFILE_SCOPE("executing stolen Job");
						stolen.Execute();
						_job_pool->DestroyObject(const_cast<Job*>(&stolen.GetJob()));
					}
					else
					{
						if (stolen.IsValid() && stolen.GetJob().IsEnabled())
						{
							AddJob(NormalizePriority(stolen.GetPriority()), &stolen.GetJob());
						}
						else
						{
							// we did not steal a good job thus we want to steal from someone else
							_coworker_index = (_coworker_index + 1) % _coworkers.size();

							// we yield/sleep just in case all jobs are done
							concurrency::ThisThread::yield();
							concurrency::ThisThread::sleep_for(_bad_steal_sleep_duration);
						}
					}
				}
			}

			_work_procedure_complete.store(true, mo_release);
		}

		Job* CreateJob(Job::Function& function, Job* dependent = nullptr)
		{
			Job* job = nullptr;

			if (dependent)
			{
				job = _job_pool->CreateObject(function, *dependent);
			}
			else
			{
				job = _job_pool->CreateObject(function);
			}

			return job;
		}

		JobRecord AddJob(JobPriority priority, const Job* job)
		{
			return AddJob(JobRecord(priority, job));
		}

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
		JobWorker():
			_highest_job_deque(JOB_DEQUEUE_SIZE),
			_higher_job_deque(JOB_DEQUEUE_SIZE),
			_normal_job_deque(JOB_DEQUEUE_SIZE),
			_lower_job_deque(JOB_DEQUEUE_SIZE),
			_lowest_job_deque(JOB_DEQUEUE_SIZE),
			_keep_working(false),
			_work_procedure_complete(true),
			_thread_pool(nullptr),
			_coworker_index(0),
			_bad_steal_sleep_duration(NP_ENGINE_APPLICATION_LOOP_DURATION)
		{}

		~JobWorker()
		{
			StopWork();
		}

		void AddCoworker(JobWorker* coworker)
		{
			if (coworker != nullptr && coworker != this)
			{
				_coworkers.emplace_back(coworker);
			}
		}

		void RemoveCoworker(JobWorker* coworker)
		{
			for (auto it = _coworkers.begin(); it != _coworkers.end(); it++)
			{
				if (coworker == *it)
				{
					_coworkers.erase(it);
					break;
				}
			}
		}

		void ClearCorkers()
		{
			_coworkers.clear();
		}

		JobToken CreateJobToken(Job::Function& function, JobToken dependent = JobToken())
		{
			JobToken token(CreateJob(function));

			if (token.IsValid() && dependent.IsValid())
			{
				dependent.GetJob().DependOn(token.GetJob());
			}

			return token;
		}

		JobRecord AddJob(JobPriority priority, JobToken token)
		{
			JobRecord record;

			if (token.IsValid())
			{
				record = AddJob(priority, &token.GetJob());
			}

			return record;
		}

		JobRecord AddJob(JobPriority priority, Job::Function& function, JobToken dependent = JobToken())
		{
			return AddJob(priority, CreateJobToken(function, dependent));
		}

		void StartWork(concurrency::ThreadPool& pool, i32 thread_affinity = -1)
		{
			NP_PROFILE_FUNCTION();
			_thread_pool = &pool;
			_keep_working.store(true, mo_release);
			_work_procedure_complete.store(false, mo_release);
			_coworker_index = _random_engine.GetLemireWithinRange(_coworkers.size());
			_thread_token = _thread_pool->CreateThread(thread_affinity, &JobWorker::WorkerThreadProcedure, this);
		}

		void StopWork()
		{
			NP_PROFILE_FUNCTION();
			_keep_working.store(false, mo_release);

			do
			{
				concurrency::ThisThread::yield();
			}
			while (!_work_procedure_complete.load(mo_acquire));

			if (_thread_pool && _thread_token.IsValid())
			{
				_thread_pool->RemoveThread(_thread_token);
				_thread_token.Invalidate();
				_thread_pool = nullptr;
			}
		}
	};
} // namespace np::js

#endif /* NP_ENGINE_JOB_WORKER_HPP */
