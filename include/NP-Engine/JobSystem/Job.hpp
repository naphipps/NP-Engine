//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_HPP
#define NP_ENGINE_JOB_HPP

#include <utility>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Container/Container.hpp"

#include "JobPriority.hpp"

/*
	TODO: how do we build a job from concurrent jobs

	(1):
		- test waiting on a job to complete - essentially a syncronyous job that we still submit... use the counter on the
   sptr...
		- while waiting on sptr.strong_count == 1, we can also pull sub-tasks from queue so we can work while we wait on
   everyone to finish

	(2):
	finish_job;
	start_job;
	vector<job> middle_jobs;

	for (n times)
	{
		middle_jobs[i].dependon(start_job)
		finish_job.dependon(middle_jobs[i])
		job_sys.submit(middle_jobs[i])
	}

	job_sys.submit(finish_job)
	job_sys.submit(start_job)

	^ does not matter which order we submit unless we want middle jobs to wait on start job
*/

namespace np::jsys
{
	class Job
	{
	private:
		mutexed_wrapper<con::vector<mem::sptr<Job>>> _dependents;
		atm_i32 _antecedent_count;
		mem::Delegate _delegate;

	public:
		Job(): _antecedent_count(0) {}

		Job(const Job& other) = delete;

		Job(Job&& other) noexcept:
			_dependents(::std::move(other._dependents)),
			_antecedent_count(::std::move(other._antecedent_count.load(mo_acquire))),
			_delegate(::std::move(other._delegate))
		{}

		~Job() = default;

		Job& operator=(const Job& other) = delete;

		Job& operator=(Job&& other) noexcept
		{
			_dependents = ::std::move(other._dependents);
			_antecedent_count.store(::std::move(other._antecedent_count.load(mo_acquire)), mo_release);
			_delegate = ::std::move(other._delegate);
			return *this;
		}

		void SetCallback(mem::Delegate::Callback callback)
		{
			_delegate.SetCallback(callback);
		}

		mem::Delegate::Callback GetCallback() const
		{
			return _delegate.GetCallback();
		}

		void SetPayload(void* payload)
		{
			_delegate.SetPayload(payload);
		}

		void* GetPayload() const
		{
			return _delegate.GetPayload();
		}

		bl CanExecute() const
		{
			return _antecedent_count.load(mo_acquire) == 0;
		}

		void operator()(siz worker_id)
		{
			if (CanExecute())
			{
				_delegate.SetId(worker_id);
				_delegate();
				
				{
					auto dependents = _dependents.get_access();
					for (auto it = dependents->begin(); it != dependents->end(); it++)
						(*it)->_antecedent_count.fetch_sub(1);
				}

				_antecedent_count.fetch_sub(1);
			}
		}

		bl IsComplete() const
		{
			return _antecedent_count.load(mo_acquire) == -1;
		}

		/*
			make a depend on b
		*/
		static void AddDependency(mem::sptr<Job> a, mem::sptr<Job> b)
		{
			auto dependents = b->_dependents.get_access();
			if (!a->IsComplete() && !b->IsComplete())
			{
				dependents->emplace_back(a);
				a->_antecedent_count.fetch_add(1);
			}
		}

		/*
			stop a from depending on b if it is
		*/
		static void RemoveDependency(mem::sptr<Job> a, mem::sptr<Job> b)
		{
			auto dependents = b->_dependents.get_access();
			if (!a->IsComplete() && !b->IsComplete())
			{
				for (auto it = dependents->begin(); it != dependents->end(); )
				{
					if (*it == a)
					{
						it = dependents->erase(it);
						a->_antecedent_count.fetch_sub(1);
					}
					else
					{
						it++;
					}
				}
			}
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_HPP */