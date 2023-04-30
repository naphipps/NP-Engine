//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/23/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_SYNC_TYPES_HPP
#define NP_ENGINE_SYNC_TYPES_HPP

#include <mutex>
#include <atomic>

#include "PrimitiveTypes.hpp"

namespace np
{
	using mutex = ::std::mutex; //TODO: remove this
	using lock = ::std::scoped_lock<::std::mutex>; //TODO: remove this

	template <typename T>
	class mutexed_wrapper
	{
	protected:
		mutex _m;
		T _object;

	public:
		class access
		{
		protected:
			lock _l;
			T& _reference;

		public:
			access(mutex& m, T& reference) : _l(m), _reference(reference) {}

			T& operator*() const
			{
				return _reference;
			}

			T* operator->() const
			{
				return &_reference;
			}
		};

		template <typename... Args>
		mutexed_wrapper(Args&&... args) : _object(::std::forward<Args>(args)...) {}

		access get_access()
		{
			return { _m, _object };
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
