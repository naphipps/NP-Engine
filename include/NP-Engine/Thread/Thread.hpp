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

// TODO: snake case this layer

namespace np::thr
{
	class Thread
	{
	public:
		using Id = ::std::thread::id;

		constexpr static siz ALIGNMENT = mem::DEFAULT_ALIGNMENT;

		static inline ui32 HardwareConcurrency() noexcept
		{
			return ::std::thread::hardware_concurrency();
		}

	private:
		using StdThreadBlock = mem::sized_block<sizeof(::std::thread), ALIGNMENT>;
		mutexed_wrapper<StdThreadBlock> _thread_block;

		void Zeroize(StdThreadBlock& thread_block)
		{
			mem::block b = thread_block;
			b.zeroize();
		}

		bl HasThread(StdThreadBlock& thread_block) const
		{
			bl has_thread = false;
			for (siz i = 0; i < StdThreadBlock::SIZE && !has_thread; i++)
				has_thread |= thread_block.allocation[i] != 0;
			return has_thread;
		}

		::std::thread* GetThread(StdThreadBlock& thread_block) const
		{
			return HasThread(thread_block) ? (::std::thread*)thread_block.allocation : nullptr;
		}

	public:
		Thread()
		{
			Zeroize(*_thread_block.get_access());
		}

		~Thread()
		{
			Join();
		}

		template <typename... Args>
		void Run(Args&&... args)
		{
			Join();
			auto thread_access = _thread_block.get_access();
			mem::construct<::std::thread>(*thread_access, ::std::forward<Args>(args)...);
		}

		void Join()
		{
			auto thread_access = _thread_block.get_access();
			::std::thread* thread = GetThread(*thread_access);
			if (thread)
			{
				thread->join();
				mem::destruct<::std::thread>(thread);
				Zeroize(*thread_access);
			}
		}

		bl SetAffinity(siz core_number);

		Id GetId()
		{
			auto thread_access = _thread_block.get_access();
			::std::thread* thread = GetThread(*thread_access);
			return thread ? thread->get_id() : Id{};
		}
	};

	namespace ThisThread
	{
		using namespace ::std::this_thread;

		bl SetAffinity(siz core_number);
	} // namespace ThisThread

	using ThreadPool = mem::object_pool<Thread, Thread::ALIGNMENT>;
} // namespace np::thr

#endif /* NP_ENGINE_THREAD_HPP */
