//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 8/25/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_THR_THREAD_HPP
#define NP_ENGINE_THR_THREAD_HPP

#include <utility>
#include <thread>

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Platform/Platform.hpp"
#include "NP-Engine/Memory/Memory.hpp"
#include "NP-Engine/Time/Time.hpp"

namespace np::thr
{
	class thread
	{
	public:
		using id = ::std::thread::id;

		constexpr static siz ALIGNMENT = mem::DEFAULT_ALIGNMENT;

		static inline ui32 hardware_concurrency() noexcept
		{
			return ::std::thread::hardware_concurrency();
		}

	protected:
		using std_thread_block = mem::sized_block<sizeof(::std::thread), ALIGNMENT>;
		mutexed_wrapper<std_thread_block> _thread_block;

		void zeroize(std_thread_block& thread_block)
		{
			mem::block b = thread_block;
			b.zeroize();
		}

		bl has_thread(std_thread_block& thread_block) const
		{
			bl has_thread = false;
			for (siz i = 0; i < std_thread_block::SIZE && !has_thread; i++)
				has_thread |= thread_block.allocation[i] != 0;
			return has_thread;
		}

		::std::thread* get_thread(std_thread_block& thread_block) const
		{
			return has_thread(thread_block) ? (::std::thread*)thread_block.allocation : nullptr;
		}

	public:
		thread()
		{
			zeroize(*_thread_block.get_access());
		}

		~thread()
		{
			join();
		}

		template <typename... Args>
		void run(Args&&... args)
		{
			join();
			auto thread_access = _thread_block.get_access();
			mem::construct<::std::thread>(*thread_access, ::std::forward<Args>(args)...);
		}

		void join()
		{
			auto thread_access = _thread_block.get_access();
			::std::thread* thread = get_thread(*thread_access);
			if (thread)
			{
				thread->join();
				mem::destruct<::std::thread>(thread);
				zeroize(*thread_access);
			}
		}

		bl set_affinity(siz core_number);

		id get_id()
		{
			auto thread_access = _thread_block.get_access();
			::std::thread* thread = get_thread(*thread_access);
			return thread ? thread->get_id() : id{};
		}
	};

	namespace this_thread
	{
		using namespace ::std::this_thread;

		bl set_affinity(siz core_number);
	} // namespace this_thread

	using thread_pool = mem::object_pool<thread, thread::ALIGNMENT>;
} // namespace np::thr

#endif /* NP_ENGINE_THR_THREAD_HPP */
