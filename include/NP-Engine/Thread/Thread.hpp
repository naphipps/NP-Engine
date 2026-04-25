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

	enum class sleep_type : ui32
	{
		std = 1,
		busy,
		hybrid
	};

	class thread_duration_sleeper
	{
	public:
		constexpr static dbl DEFAULT_OVERHEAD_SCALAR = 0.05;
		constexpr static dbl DEFAULT_HYBRID_RATIO = 0.97;

	protected:
		tim::milliseconds _duration;
		tim::steady_timestamp _previous;
		tim::milliseconds _overhead;
		dbl _scalar;
		dbl _ratio;

	public:
		thread_duration_sleeper(tim::milliseconds duration) :
			_duration(duration),
			_previous(tim::steady_clock::now()),
			_overhead{},
			_scalar(DEFAULT_OVERHEAD_SCALAR),
			_ratio(DEFAULT_HYBRID_RATIO)
		{}

		void set_overhead_scalar(dbl scalar)
		{
			_scalar = ::std::min(::std::max(scalar, 0.0), 1.0);
		}

		void set_hybrid_ratio(dbl ratio)
		{
			_ratio = ::std::min(::std::max(ratio, 0.0), 1.0);
		}

		void sleep(sleep_type type = sleep_type::hybrid)
		{
			if (type == sleep_type::std || type == sleep_type::hybrid)
			{
				tim::milliseconds dur = _duration;
				if (type == sleep_type::hybrid)
					dur *= _ratio;

				dur -= (tim::steady_clock::now() - _previous);
				if (dur.count() > 0)
				{
					tim::steady_timestamp start = tim::steady_clock::now();
					this_thread::sleep_for(dur - _overhead);
					tim::milliseconds actual = tim::steady_clock::now() - start;
					_overhead += (actual - dur) * _scalar;
					_overhead = ::std::max(_overhead, tim::milliseconds{ 0 });
				}
			}

			if (type == sleep_type::busy || type == sleep_type::hybrid)
			{
				for (tim::steady_timestamp next = tim::steady_clock::now();
					next - _previous < _duration;
					next = tim::steady_clock::now())
					this_thread::yield();
			}

			_previous = tim::steady_clock::now();
		}

		/*
			resets previous timestamp and overhead duration
		*/
		void reset()
		{
			_previous = tim::steady_clock::now();
			_overhead = tim::milliseconds{};
		}
	};
} // namespace np::thr

#endif /* NP_ENGINE_THR_THREAD_HPP */
