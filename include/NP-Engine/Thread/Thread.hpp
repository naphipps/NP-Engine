//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_THREAD_HPP
#define NP_ENGINE_THREAD_HPP

#include <utility>
#include <thread>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Time/Time.hpp"

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
		using StdThreadBlock = mem::SizedBlock<sizeof(::std::thread)>;
		StdThreadBlock _thread_block;

		void ZeroizeThreadBlock()
		{
			((mem::Block)_thread_block).Zeroize();
		}

		bl HasThread() const
		{
			bl has_thread = false;
			for (siz i = 0; i < StdThreadBlock::SIZE && !has_thread; i++)
				has_thread = _thread_block.allocation[i] != 0;

			return has_thread;
		}

		::std::thread* GetThread() const
		{
			return HasThread() ? (::std::thread*)_thread_block.allocation : nullptr;
		}

	public:
		Thread()
		{
			ZeroizeThreadBlock();
		}

		~Thread()
		{
			Clear();
		}

		template <typename... Args>
		void Run(Args&&... args)
		{
			Clear();
			mem::Construct<::std::thread>(_thread_block, ::std::forward<Args>(args)...);
		}

		void Clear()
		{
			::std::thread* thread = GetThread();
			if (thread)
			{
				thread->join();
				mem::Destruct<::std::thread>(thread);
				ZeroizeThreadBlock();
			}
		}

		bl SetAffinity(siz core_number);

		bl IsRunning() const
		{
			return HasThread();
		}

		Id GetId() const
		{
			::std::thread* thread = GetThread();
			return thread ? thread->get_id() : Id();
		}
	};

	namespace ThisThread
	{
		using namespace ::std::this_thread;

		bl SetAffinity(siz core_number);
	} // namespace ThisThread

	using ThreadPool = mem::ObjectPool<Thread>;
} // namespace np::thr

#endif /* NP_ENGINE_THREAD_HPP */
