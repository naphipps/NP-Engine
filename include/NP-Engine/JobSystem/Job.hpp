//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_JOB_HPP
#define NP_ENGINE_JOB_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Insight/Insight.hpp"
#include "NP-Engine/Concurrency/Concurrency.hpp"
#include "NP-Engine/Memory/Memory.hpp"

// TODO: replace copy constructor with move constructor
// TODO: replace "Job* _dependent_job;" with "vector<Job*>* _dependent_jobs;"

namespace np::js
{
	class Job
	{
	public:
		using Function = memory::Delegate;

	private:
		Job* _dependent_job; // job depending on this
		atm_i32 _dependency_count;
		Function _function;

		void Dispose()
		{
			if (_dependent_job && !IsComplete())
			{
				_dependent_job->_dependency_count--;
			}

			_dependent_job = nullptr;
			_function.DisconnectFunction();
			_dependency_count = -1;
		}

		inline void CopyFrom(const Job& other)
		{
			_dependency_count.store(other._dependency_count);

			if (other.IsEnabled())
			{
				_dependent_job = other._dependent_job;
				_function = other._function;
			}
		}

	public:
		Job(): _dependent_job(nullptr), _dependency_count(-1) {}

		Job(Function& function, Job& dependent): Job(function)
		{
			dependent.DependOn(*this);
		}

		Job(Function& function): _dependent_job(nullptr), _dependency_count(1), _function(function) {}

		Job(Job& other)
		{
			CopyFrom(other);
		}

		Job(Job&&) = delete;

		~Job()
		{
			Dispose();
		}

		Job& operator=(const Job& other)
		{
			if (_dependent_job && !IsComplete())
			{
				_dependent_job->_dependency_count--;
				_dependent_job = nullptr;
			}

			NP_ENGINE_ASSERT(CanExecute(),
							 "You are overwriting a Job that has dependents. Only overwrite jobs that can execute.");

			CopyFrom(other);
			return *this;
		}

		Job& operator=(Job&&) = delete;

		bl CanExecute() const
		{
			return _dependency_count.load(mo_acquire) == 1;
		}

		void Execute()
		{
			if (CanExecute())
			{
				_function.InvokeConnectedFunction();
				_dependency_count--;

				if (_dependent_job)
				{
					_dependent_job->_dependency_count--;
				}
			}
		}

		bl IsComplete() const
		{
			return _dependency_count.load(mo_acquire) == 0;
		}

		bl IsEnabled() const
		{
			return _dependency_count.load(mo_acquire) > -1;
		}

		inline explicit operator bl() const
		{
			return IsEnabled();
		}

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
} // namespace np::js

#endif /* NP_ENGINE_JOB_HPP */
