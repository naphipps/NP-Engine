//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 1/25/22
//
//##===----------------------------------------------------------------------===##//

#include "NP-Engine/Concurrency/Thread.hpp"

#if NP_ENGINE_PLATFORM_IS_LINUX

    // TODO: cleanup headers
#include <pthread.h> //pthread_setaffinity_np
#include <unistd.h> //may not be needed
#include <sched.h> //cpu_set_t

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
    #include <Windows.h>

#endif

namespace np::concurrency
{
	bl Thread::SetAffinity(const i32 core_number)
	{
        // Apple does NOT support thread affinity - not even the pthread.h implementation supports it
#if NP_ENGINE_PLATFORM_IS_APPLE
        return false;
#else
        bl set = false;

        if (!IsThreadAllocationClear() && core_number > -1)
        {
#if NP_ENGINE_PLATFORM_IS_LINUX
            //TODO: implement something like this...
            ::std::thread* std_thread = GetStdThreadPtr();
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET((ui32)core_number, &cpuset);
            i32 rc = pthread_setaffinity_np(std_thread->native_handle(), sizeof(cpu_set_t), &cpuset);

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
            ::std::thread* std_thread = GetStdThreadPtr();
            SetThreadIdealProcessor((HANDLE)std_thread->native_handle(), (DWORD)core_number);
            SetThreadAffinityMask((HANDLE)std_thread->native_handle(), (DWORD_PTR)((ui64)1 << (ui64)core_number));
            set = true;

#endif
#endif
        }

        return set;
	}

    namespace ThisThread
    {
        bl SetAffinity(const i32 core_number)
        {
            // Apple does NOT support thread affinity - not even the pthread.h implementation supports it
#if NP_ENGINE_PLATFORM_IS_APPLE
            return false;
#else
            bl set = false;

            if (core_number > -1)
            {
#if NP_ENGINE_PLATFORM_IS_LINUX
                //TODO: implement something like above

#elif NP_ENGINE_PLATFORM_IS_WINDOWS
                HANDLE current_thread = GetCurrentThread();
                SetThreadIdealProcessor(current_thread, (DWORD)core_number);
                SetThreadAffinityMask(current_thread, (DWORD_PTR)((ui64)1 << (ui64)core_number));
                set = true;

#endif
#endif
            }

            return set;
        }
    }
}