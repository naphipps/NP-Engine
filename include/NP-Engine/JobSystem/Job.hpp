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
		con::vector<mem::wptr<Job>> _dependents;
		atm_i32 _antecedent_count;
		mem::Delegate _delegate;
		bl _can_be_stolen;

	public:
		Job(): _antecedent_count(0), _can_be_stolen(true) {}

		Job(mem::Delegate& d): _antecedent_count(0), _delegate(d), _can_be_stolen(true) {}

		Job(const Job& other) = delete;

		Job(Job&& other) noexcept:
			_dependents(::std::move(other._dependents)),
			_antecedent_count(::std::move(other._antecedent_count.load(mo_acquire))),
			_delegate(::std::move(other._delegate)),
			_can_be_stolen(::std::move(other._can_be_stolen))
		{}

		~Job()
		{
			Reset();
		}

		Job& operator=(const Job& other) = delete;

		Job& operator=(Job&& other) noexcept
		{
			_dependents = ::std::move(other._dependents);
			_antecedent_count.store(::std::move(other._antecedent_count.load(mo_acquire)), mo_release);
			_delegate = ::std::move(other._delegate);
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
			_delegate.UnsetCallback();
			_can_be_stolen = true;
		}

		bl CanExecute() const
		{
			return _antecedent_count.load(mo_acquire) == 0;
		}

		void operator()()
		{
			if (CanExecute())
			{
				_delegate();
				_antecedent_count--;

				for (auto it = _dependents.begin(); it != _dependents.end(); it++)
				{
					mem::sptr<Job> dependent = it->lock();
					if (dependent)
						dependent->_antecedent_count--;
				}
			}
		}

		bl IsComplete() const
		{
			return _antecedent_count.load(mo_acquire) == -1;
		}

		bl IsEnabled() const
		{
			return _antecedent_count.load(mo_acquire) > -1;
		}

		/*
			make a depend on b
		*/
		static void AddDependency(mem::sptr<Job> a, mem::sptr<Job> b)
		{
			if (a->IsEnabled() && b->IsEnabled())
			{
				bl found = false;
				con::vector<mem::wptr<Job>>& dependents = b->_dependents;
				for (auto it = dependents.begin(); it != dependents.end() && !found; it++)
				{
					mem::sptr<Job> dependent = it->lock();
					found = dependent && dependent.get() == a.get();
				}

				if (!found)
				{
					dependents.emplace_back(a);
					a->_antecedent_count++;
				}
			}
		}

		/*
			stop a from depending on b if it is
		*/
		static void RemoveDependency(mem::sptr<Job> a, mem::sptr<Job> b)
		{
			if (a->IsEnabled() && b->IsEnabled())
			{
				con::vector<mem::wptr<Job>>& dependents = b->_dependents;
				for (auto it = dependents.begin(); it != dependents.end(); it++)
				{
					mem::sptr<Job> dependent = it->lock();
					if (dependent.get() == a.get())
					{
						dependent.reset();
						dependents.erase(it);
						a->_antecedent_count--;
						break;
					}
				}
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