//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/25/22
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Thread/Thread.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX
	#include <pthread.h> //pthread_setaffinity_np
	#include <sched.h> //cpu_set_t

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
	#include <Windows.h>
	#include <synchapi.h>

#endif

namespace np::thr
{
	bl Thread::SetAffinity(siz core_number)
	{
#if NP_ENGINE_PLATFORM_IS_APPLE
		// Apple does NOT support thread affinity - not even their pthread.h implementation supports it
		return false;
#else
		bl set = false;

		if (!IsThreadAllocationClear() && core_number > -1)
		{
			core_number = core_number % Thread::HardwareConcurrency();

	#if NP_ENGINE_PLATFORM_IS_LINUX
			::std::thread* std_thread = GetStdThreadPtr();
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET((ui32)core_number, &cpuset);
			set = 0 == pthread_setaffinity_np(std_thread->native_handle(), sizeof(cpu_set_t), &cpuset);

	#elif NP_ENGINE_PLATFORM_IS_WINDOWS
			::std::thread* std_thread = GetStdThreadPtr();
			SetThreadIdealProcessor((HANDLE)std_thread->native_handle(), (DWORD)core_number);
			SetThreadAffinityMask((HANDLE)std_thread->native_handle(), (DWORD_PTR)((ui64)1 << (ui64)core_number));
			set = true;

	#endif
		}
		return set;
#endif
	}

	namespace ThisThread
	{
		bl SetAffinity(siz core_number)
		{
#if NP_ENGINE_PLATFORM_IS_APPLE
			// Apple does NOT support thread affinity - not even their pthread.h implementation supports it
			return false;
#else
			bl set = false;

			if (core_number > -1)
			{
				core_number = core_number % Thread::HardwareConcurrency();

	#if NP_ENGINE_PLATFORM_IS_LINUX
				cpu_set_t cpuset;
				CPU_ZERO(&cpuset);
				CPU_SET((ui32)core_number, &cpuset);
				set = 0 == pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);

	#elif NP_ENGINE_PLATFORM_IS_WINDOWS
				HANDLE current_thread = GetCurrentThread();
				SetThreadIdealProcessor(current_thread, (DWORD)core_number);
				SetThreadAffinityMask(current_thread, (DWORD_PTR)((ui64)1 << (ui64)core_number));
				set = true;
	#endif
			}
			return set;
#endif
		}
	} // namespace ThisThread
} // namespace np::thr