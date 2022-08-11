//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_THREAD_POOL_HPP
#define NP_ENGINE_THREAD_POOL_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Insight/Insight.hpp"

#include "Thread.hpp"
#include "ThreadToken.hpp"

namespace np::concurrency
{
	class ThreadPool : protected memory::ObjectPool<Thread>
	{
	private:
		using base = memory::ObjectPool<Thread>;

	public:
		constexpr static ui64 MAX_THREAD_COUNT = 128;

	public:
		ThreadPool(): base(Thread::HardwareConcurrency() < MAX_THREAD_COUNT ? Thread::HardwareConcurrency() : MAX_THREAD_COUNT)
		{}

		siz ObjectCount() const
		{
			return base::ObjectCount();
		}

		void Clear()
		{
			base::Clear();
		}

		template <class Function, class... Args>
		ThreadToken CreateThread(Function&& f, Args&&... args)
		{
			return CreateThread(-1, f, args...);
		}

		template <class Function, class... Args>
		ThreadToken CreateThread(i32 thread_affinity, Function&& f, Args&&... args)
		{
			NP_ENGINE_PROFILE_FUNCTION();

			Thread* thread = base::CreateObject();
			thread->Run(f, args...);

			if (thread_affinity > -1)
				thread->SetAffinity(thread_affinity);

			ThreadToken token(thread);
			return token;
		}

		bl RemoveThread(ThreadToken token)
		{
			return DestroyObject(&token.GetThread());
		}
	};
} // namespace np::concurrency

#endif /* NP_ENGINE_THREAD_POOL_HPP */
