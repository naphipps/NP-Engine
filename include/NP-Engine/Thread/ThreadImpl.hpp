//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_THREAD_IMPL_HPP
#define NP_ENGINE_THREAD_IMPL_HPP

#include <thread>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"

namespace np::thr
{
	class Thread
	{
	public:
		using Id = ::std::thread::id;

		static inline ui32 HardwareConcurrency() noexcept
		{
			return ::std::thread::hardware_concurrency();
		}

	private:
		ui8 _thread_allocation[sizeof(::std::thread)];

		void ClearThreadAllocation()
		{
			for (siz i = 0; i < sizeof(::std::thread); i++)
				_thread_allocation[i] = 0;
		}

		bl IsThreadAllocationClear() const
		{
			bl is_clear = true;
			for (siz i = 0; i < sizeof(::std::thread) && is_clear; i++)
				is_clear = _thread_allocation[i] == 0;

			return is_clear;
		}

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

		template <class Function, class... Args>
		void Run(Function&& f, Args&&... args)
		{
			new (_thread_allocation)::std::thread(f, args...);
		}

		void Dispose()
		{
			if (!IsThreadAllocationClear())
			{
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

		bl SetAffinity(siz core_number);

		bl IsRunning() const
		{
			return !IsThreadAllocationClear();
		}

		Id GetId() const
		{
			return !IsThreadAllocationClear() ? GetStdThreadPtr()->get_id() : Id();
		}
	};

	namespace ThisThread
	{
		using namespace ::std::this_thread;

		bl SetAffinity(siz core_number);
	} // namespace ThisThread
} // namespace np::thr

#endif /* NP_ENGINE_THREAD_IMPL_HPP */
