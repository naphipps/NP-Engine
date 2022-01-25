//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_THREAD_HPP
#define NP_ENGINE_THREAD_HPP

#include <thread>

#if NP_ENGINE_PLATFORM_IS_LINUX

	// TODO: cleanup headers
	#include <pthread.h> //pthread_setaffinity_np
	#include <unistd.h> //may not be needed
	#include <sched.h> //cpu_set_t

#elif NP_ENGINE_PLATFORM_IS_WINDOWS

	// TODO: cleanup headers
	#include <Windows.h>

#endif

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::concurrency
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

		bl SetAffinity(const ui8 core_number)
		{
			// TODO: implement thread affinity support
			bl set = false;

			if (!IsThreadAllocationClear())
			{
                // Apple does NOT support thread affinity - not even the pthread.h implementation supports it
#if NP_ENGINE_PLATFORM_IS_LINUX
                //TODO: implement something like this...
                ::std::thread* std_thread = GetStdThreadPtr();
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                CPU_SET((ui32)core_number, &cpuset);
                i32 rc = pthread_setaffinity_np(std_thread->native_handle(), sizeof(cpu_set_t), &cpuset);
                
#elif NP_ENGINE_PLATFORM_IS_WINDOWS
                //TODO: implement something like this...
                ::std::thread* std_thread = GetStdThreadPtr();
                DWORD_PTR mask = 1ull << core_number;
                SetThreadAffinityMask(_handle, mask);
                
#endif
			}

			return set;
		}

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

		static inline bl SetAffinity(ui8 core_number)
		{
			return false; // TODO: implement
		}
	} // namespace ThisThread
} // namespace np::concurrency

#endif /* NP_ENGINE_THREAD_HPP */
