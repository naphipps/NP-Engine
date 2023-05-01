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
		StdThreadBlockAccess thread_access = _thread_block.get_access();

		bl set = false;
		
#if NP_ENGINE_PLATFORM_IS_APPLE
		// Apple does NOT support thread affinity - not even their pthread.h implementation supports it

#elif NP_ENGINE_PLATFORM_IS_LINUX
		core_number = core_number % Thread::HardwareConcurrency();
		::std::thread* thread = GetThread(*thread_access);
		if (thread)
		{
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET((ui32)core_number, &cpuset);
			set = 0 == pthread_setaffinity_np(thread->native_handle(), sizeof(cpu_set_t), &cpuset);
		}

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
		core_number = core_number % Thread::HardwareConcurrency();
		::std::thread* thread = GetThread(*thread_access);
		if (thread)
		{
			SetThreadIdealProcessor((HANDLE)thread->native_handle(), (DWORD)core_number);
			SetThreadAffinityMask((HANDLE)thread->native_handle(), (DWORD_PTR)((ui64)1 << (ui64)core_number));
			set = true;
		}

#endif
		return set;
	}

	namespace ThisThread
	{
		bl SetAffinity(siz core_number)
		{
			bl set = false;
#if NP_ENGINE_PLATFORM_IS_APPLE
			// Apple does NOT support thread affinity - not even their pthread.h implementation supports it

#elif NP_ENGINE_PLATFORM_IS_LINUX
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
			return set;
		}
	} // namespace ThisThread
} // namespace np::thr