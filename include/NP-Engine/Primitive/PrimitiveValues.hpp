//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/7/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PRIMITIVE_VALUES_HPP
#define NP_ENGINE_PRIMITIVE_VALUES_HPP

#include "PrimitiveTypes.hpp"

#define BIT(n) ((::np::ui64)1 << (::np::ui64)n)
#define KILOBYTE_SIZE ((::np::ui64)1000)
#define MEGABYTE_SIZE ((::np::ui64)1000000)
#define GIGABYTE_SIZE ((::np::ui64)1000000000)
#define TERABYTE_SIZE ((::np::ui64)1000000000000)

#define KIBIBYTE_SIZE ((::np::ui64)1024)
#define MEBIBYTE_SIZE ((::np::ui64)1048576)
#define GIBIBYTE_SIZE ((::np::ui64)1073741824)
#define TEBIBYTE_SIZE ((::np::ui64)1099511627776)

#define BIT_COUNT(T) (sizeof(T) * 8)
#define UI8_BIT_COUNT BIT_COUNT(::np::ui8)
#define UI16_BIT_COUNT BIT_COUNT(::np::ui16)
#define UI32_BIT_COUNT BIT_COUNT(::np::ui32)
#define UI64_BIT_COUNT BIT_COUNT(::np::ui64)
#define I8_BIT_COUNT BIT_COUNT(::np::i8)
#define I16_BIT_COUNT BIT_COUNT(::np::i16)
#define I32_BIT_COUNT BIT_COUNT(::np::i32)
#define I64_BIT_COUNT BIT_COUNT(::np::i64)
#define SIZ_BIT_COUNT BIT_COUNT(::np::siz)
#define FLT_BIT_COUNT BIT_COUNT(::np::flt)
#define DBL_BIT_COUNT BIT_COUNT(::np::dbl)
//TODO: add bit count for: chr, uchr, wchr, chr16, chr32

#endif /* NP_ENGINE_PRIMITIVE_VALUES_HPP */
