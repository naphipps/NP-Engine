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
		con::vector<Job*> _dependents;
		atm_i32 _antecedent_count;
		mem::Delegate _delegate;
		atm<atm_bl*> _confirm_completion_flag;
		bl _can_be_stolen;

	public:
		Job(): _antecedent_count(0), _confirm_completion_flag(nullptr), _can_be_stolen(true) {}

		Job(mem::Delegate& d):
			_antecedent_count(0),
			_delegate(d),
			_confirm_completion_flag(nullptr),
			_can_be_stolen(true)
		{}

		Job(const Job& other):
			_dependents(other._dependents),
			_antecedent_count(other._antecedent_count.load(mo_acquire)),
			_delegate(other._delegate),
			_confirm_completion_flag(other._confirm_completion_flag.load(mo_acquire)),
			_can_be_stolen(other._can_be_stolen)
		{}

		Job(Job&& other) noexcept:
			_dependents(::std::move(other._dependents)),
			_antecedent_count(::std::move(other._antecedent_count.load(mo_acquire))),
			_delegate(::std::move(other._delegate)),
			_confirm_completion_flag(::std::move(other._confirm_completion_flag.load(mo_acquire))),
			_can_be_stolen(::std::move(other._can_be_stolen))
		{}

		~Job()
		{
			Reset();
		}

		Job& operator=(const Job& other)
		{
			_dependents = other._dependents;
			_antecedent_count.store(other._antecedent_count.load(mo_acquire), mo_release);
			_delegate = other._delegate;
			_confirm_completion_flag.store(other._confirm_completion_flag.load(mo_acquire), mo_release);
			_can_be_stolen = other._can_be_stolen;
			return *this;
		}

		Job& operator=(Job&& other) noexcept
		{
			_dependents = ::std::move(other._dependents);
			_antecedent_count.store(::std::move(other._antecedent_count.load(mo_acquire)), mo_release);
			_delegate = ::std::move(other._delegate);
			_confirm_completion_flag.store(::std::move(other._confirm_completion_flag.load(mo_acquire)), mo_release);
			_can_be_stolen = ::std::move(other._can_be_stolen);
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
		
		void SetDelegate(mem::Delegate& d)
		{
			_delegate = d;
		}
		
		void Reset()
		{
			_dependents.clear();
			_antecedent_count.store(0, mo_release);
			_delegate.DisconnectFunction();
			_confirm_completion_flag.store(nullptr, mo_release);
			_can_be_stolen = true;
		}

		bl CanExecute() const
		{
			return _antecedent_count.load(mo_acquire) == 0;
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
			return _antecedent_count.load(mo_acquire) == -1;
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

		void SetCanBeStolen(bl can_be_stolen)
		{
			_can_be_stolen = can_be_stolen;
		}

		bl CanBeStolen() const
		{
			return _can_be_stolen;
		}
	};
} // namespace np::jsys

#endif /* NP_ENGINE_JOB_HPP */