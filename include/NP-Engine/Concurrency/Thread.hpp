//
//  Thread.hpp
//  Project Space
//
//  Created by Nathan Phipps on 8/25/20.
//  Copyright © 2020 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_THREAD_HPP
#define NP_ENGINE_THREAD_HPP

#include <thread>

#include "NP-Engine/Primitive/Primitive.hpp"

// TODO: update comments

namespace np
{
	namespace concurrency
	{
		/**
		 represent and wraps a std thread
		 */
		class Thread
		{
		public:
			using Id = ::std::thread::id;

			/**
			 gets the hardware concurrency
			 */
			static inline ui32 HardwareConcurrency() noexcept
			{
				return ::std::thread::hardware_concurrency();
			}

		private:
			ui8 _thread_allocation[sizeof(::std::thread)];

			void ClearThreadAllocation()
			{
				for (siz i = 0; i < sizeof(::std::thread); i++)
				{
					_thread_allocation[i] = 0;
				}
			}

			bl IsThreadAllocationClear() const
			{
				bl is_clear = true;

				for (siz i = 0; i < sizeof(::std::thread); i++)
				{
					if (_thread_allocation[i] != 0)
					{
						is_clear = false;
						break;
					}
				}

				return is_clear;
			}

			/**
			 gets the std thread ptr from our allocation
			 */
			::std::thread* GetStdThreadPtr() const
			{
				return (::std::thread*)_thread_allocation;
			}

		public:
			Thread()
			{
				ClearThreadAllocation();
			}

			~Thread()
			{
				Dispose();
			}

			/**
			 starts our Thread instance with the given function and args
			 */
			template <class Function, class... Args>
			void Run(Function&& f, Args&&... args)
			{
				new (_thread_allocation)::std::thread(f, args...);
			}

			/**
			 safely disposed of std Thread
			 */
			void Dispose()
			{
				if (!IsThreadAllocationClear())
				{
					// TODO: do we need to handle affinity here??

					// deallocate std::Thread -- should always be joinable here
					::std::thread* std_thread = GetStdThreadPtr();
					if (std_thread->joinable())
					{
						std_thread->join();
						std_thread->::std::thread::~thread();
						ClearThreadAllocation();
					}
				}
			}

			/**
			 sets the Thread affinity given the core number
			 */
			bl SetAffinity(const ui8 core_number)
			{
				// TODO: support affinity for all main OS's, then consoles if they support?? Switch, Xbox, PlayStation
				return false;
			}

			/**
			 checks if our Thread is running or not
			 */
			bl IsRunning() const
			{
				return !IsThreadAllocationClear();
			}

			/**
			 get our inner std thread's id
			 when our inner std thread is null, then we return a zeroed thread id
			 */
			Id GetId() const
			{
				return !IsThreadAllocationClear() ? GetStdThreadPtr()->get_id() : Id();
			}
		};

		/**
		 represents std this_thread
		 */
		namespace ThisThread
		{
			using namespace ::std::this_thread;
		} // namespace ThisThread
	} // namespace concurrency
} // namespace np

#endif /* NP_ENGINE_THREAD_HPP */
