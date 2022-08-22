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

namespace np::jsys
{
	class Job
	{
	private:
		container::vector<Job*> _dependents;
		atm_i32 _antecedent_count;
		mem::Delegate _delegate;
		atm<atm_bl*> _confirm_completion_flag;
		JobPriority _priority_attractor;

	public:
		Job(): _antecedent_count(-1), _confirm_completion_flag(nullptr), _priority_attractor(JobPriority::Normal) {}

		Job(mem::Delegate&& d):
			_antecedent_count(1),
			_delegate(d),
			_confirm_completion_flag(nullptr),
			_priority_attractor(JobPriority::Normal)
		{}

		Job(const Job& other):
			_dependents(other._dependents),
			_antecedent_count(other._antecedent_count.load(mo_acquire)),
			_delegate(other._delegate),
			_confirm_completion_flag(other._confirm_completion_flag.load(mo_acquire)),
			_priority_attractor(other._priority_attractor)
		{}

		Job(Job&& other) noexcept:
			_dependents(::std::move(other._dependents)),
			_antecedent_count(::std::move(other._antecedent_count.load(mo_acquire))),
			_delegate(::std::move(other._delegate)),
			_confirm_completion_flag(::std::move(other._confirm_completion_flag.load(mo_acquire))),
			_priority_attractor(::std::move(other._priority_attractor))
		{}

		~Job()
		{
			Dispose();
		}

		Job& operator=(const Job& other)
		{
			_dependents = other._dependents;
			_antecedent_count.store(other._antecedent_count.load(mo_acquire), mo_release);
			_delegate = other._delegate;
			_confirm_completion_flag.store(other._confirm_completion_flag.load(mo_acquire), mo_release);
			_priority_attractor = other._priority_attractor;
			return *this;
		}

		Job& operator=(Job&& other) noexcept
		{
			_dependents = ::std::move(other._dependents);
			_antecedent_count.store(::std::move(other._antecedent_count.load(mo_acquire)), mo_release);
			_delegate = ::std::move(other._delegate);
			_confirm_completion_flag.store(::std::move(other._confirm_completion_flag.load(mo_acquire)), mo_release);
			_priority_attractor = ::std::move(other._priority_attractor);
			return *this;
		}

		mem::Delegate& GetDelegate()
		{
			return _delegate;
		}

		const mem::Delegate& GetDelegate() const
		{
			return _delegate;
		}

		void Dispose()
		{
			_dependents.clear();
			_antecedent_count.store(-1, mo_release);
			_delegate.DisconnectFunction();
			_confirm_completion_flag.store(nullptr, mo_release);
			_priority_attractor = JobPriority::Normal;
		}

		bl CanExecute() const
		{
			return _antecedent_count.load(mo_acquire) == 1;
		}

		void Execute()
		{
			if (CanExecute())
			{
				_delegate.InvokeConnectedFunction();
				atm_bl* flag = _confirm_completion_flag.load(mo_acquire);

				if (!flag || flag->load(mo_acquire))
				{
					_antecedent_count--;
					for (auto it = _dependents.begin(); it != _dependents.end(); it++)
						(*it)->_antecedent_count--;
				}
			}
		}

		void WatchConfirmCompletionFlag(atm_bl* completion_flag)
		{
			_confirm_completion_flag.store(completion_flag, mo_release);
		}

		void UnwatchConfirmCompletionFlag()
		{
			_confirm_completion_flag.store(nullptr, mo_release);
		}

		bl IsComplete() const
		{
			return _antecedent_count.load(mo_acquire) == 0;
		}

		bl IsEnabled() const
		{
			return _antecedent_count.load(mo_acquire) > -1;
		}

		void AddDependency(Job& other)
		{
			if (IsEnabled())
			{
				bl found = false;
				for (auto it = other._dependents.begin(); it != other._dependents.end() && !found; it++)
					found = *it == this;

				if (!found)
				{
					other._dependents.emplace_back(this);
					_antecedent_count++;
				}
			}
		}

		void RemoveDependency(Job& other)
		{
			if (IsEnabled())
				for (auto it = other._dependents.begin(); it != other._dependents.end(); it++)
					if (*it == this)
					{
						other._dependents.erase(it);
						_antecedent_count--;
						break;
					}
		}

		void SetPriorityAttractor(JobPriority priority)
		{
			_priority_attractor = priority;
		}

		void ResetPriorityAttractor()
		{
			_priority_attractor = JobPriority::Normal;
		}

		JobPriority GetAttractedPriority(JobPriority priority)
		{
			ui32 attractor = (ui32)_priority_attractor;
			ui32 priority_ui32 = (ui32)priority;

			if (priority_ui32 < attractor)
				priority_ui32++;
			else if (priority_ui32 > attractor)
				priority_ui32--;

			return (JobPriority)priority_ui32;
		}
	};
} // namespace np::js

#endif /* NP_ENGINE_JOB_HPP */