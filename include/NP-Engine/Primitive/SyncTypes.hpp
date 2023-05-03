//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/23/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SYNC_TYPES_HPP
#define NP_ENGINE_SYNC_TYPES_HPP

#include <mutex>
#include <atomic>
#include <chrono>
#include <thread>

#include "PrimitiveTypes.hpp"

namespace np
{
	using mutex = ::std::mutex; //TODO: remove this
	using lock = ::std::scoped_lock<::std::mutex>; //TODO: remove this

	template <typename T>
	class mutexed_wrapper
	{
	protected:
		::std::mutex _m;
		T _object;

	public:
		class access
		{
		protected:
			::std::unique_lock<::std::mutex> _l;
			T* _object;

		public:
			access(T* object, ::std::mutex& m) : _l(m), _object(object) {}

			template<class R, class P>
			access(T* object, ::std::mutex& m, const ::std::chrono::duration<R, P> duration) : _l(m, ::std::defer_lock), _object(nullptr)
			{
				auto start = ::std::chrono::steady_clock::now();
				while ((::std::chrono::steady_clock::now() - start) < duration && !_l.try_lock())
					::std::this_thread::yield();

				if (_l)
					_object = object;
			}

			operator bl() const
			{
				return _object;
			}

			T& operator*() const
			{
				return *_object;
			}

			T* operator->() const
			{
				return _object;
			}
		};

		template <typename... Args>
		mutexed_wrapper(Args&&... args) : _object(::std::forward<Args>(args)...) {}

		access get_access() //TODO: add param for a timeout - this means _renderence will need to be a ptr and we'll need to add a bl operator to access
		{
			return { &_object, _m };
		}

		template<class R, class P>
		access try_get_access_for(const ::std::chrono::duration<R, P> duration)
		{
			return { &_object, _m, duration };
		}
	};

	template <typename T>
	using atm = ::std::atomic<T>;

	using atm_ui8 = ::std::atomic_uint8_t;
	using atm_ui16 = ::std::atomic_uint16_t;
	using atm_ui32 = ::std::atomic_uint32_t;
	using atm_ui64 = ::std::atomic_uint64_t;

	using atm_i8 = ::std::atomic_int8_t;
	using atm_i16 = ::std::atomic_int16_t;
	using atm_i32 = ::std::atomic_int32_t;
	using atm_i64 = ::std::atomic_int64_t;
	using atm_siz = ::std::atomic_size_t;

	using atm_flt = ::std::atomic<flt>;
	using atm_dbl = ::std::atomic<dbl>;

	using atm_chr = ::std::atomic_char;
	using atm_uchr = ::std::atomic_uchar;

	using atm_bl = ::std::atomic_bool;

	inline constexpr ::std::memory_order mo_relaxed = ::std::memory_order_relaxed;
	inline constexpr ::std::memory_order mo_consume = ::std::memory_order_consume;
	inline constexpr ::std::memory_order mo_acquire = ::std::memory_order_acquire;
	inline constexpr ::std::memory_order mo_release = ::std::memory_order_release;
	inline constexpr ::std::memory_order mo_acq_rel = ::std::memory_order_acq_rel;
	inline constexpr ::std::memory_order mo_seq_cst = ::std::memory_order_seq_cst;
}

#endif /* NP_ENGINE_SYNC_TYPES_HPP */
