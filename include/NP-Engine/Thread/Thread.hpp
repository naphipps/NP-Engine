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
		constexpr static siz THREAD_SIZE = mem::CalcAlignedSize(sizeof(::std::thread));
		ui8 _thread_allocation[THREAD_SIZE];

		void ZeroizeThreadBlock()
		{
			mem::Block block = GetThreadBlock();
			block.Zeroize();
		}

		bl HasThread() const
		{
			bl has_thread = false;
			for (siz i = 0; i < THREAD_SIZE && !has_thread; i++)
				has_thread = _thread_allocation[i] != 0;

			return has_thread;
		}

		::std::thread* GetThread() const
		{
			return HasThread() ? (::std::thread*)_thread_allocation : nullptr;
		}

		mem::Block GetThreadBlock() const
		{
			return { (void*)_thread_allocation, THREAD_SIZE };
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
			mem::Block block = GetThreadBlock();
			mem::Construct<::std::thread>(block, ::std::forward<Args>(args)...);
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
