//
//  atomic_types.hpp
//  Project Space
//
//  Created by Nathan Phipps on 1/3/21.
//  Copyright © 2021 Nathan Phipps. All rights reserved.
//

#ifndef NP_ENGINE_ATOMIC_TYPES_HPP
#define NP_ENGINE_ATOMIC_TYPES_HPP

#include <atomic>

#include "primitive_types.hpp"

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

#endif /* NP_ENGINE_ATOMIC_TYPES_HPP */
