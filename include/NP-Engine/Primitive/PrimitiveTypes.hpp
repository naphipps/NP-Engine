//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/30/20
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_PRIMITIVE_TYPES_HPP
#define NP_ENGINE_PRIMITIVE_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <cfloat>

#define UI8_MIN 0
#define UI8_MAX UINT8_MAX
#define UI16_MIN 0
#define UI16_MAX UINT16_MAX
#define UI32_MIN 0
#define UI32_MAX UINT32_MAX
#define UI64_MIN 0
#define UI64_MAX UINT64_MAX
#define SIZ_MAX UINT64_MAX
#define I8_MIN INT8_MIN
#define I8_MAX INT8_MAX
#define I16_MIN INT16_MIN
#define I16_MAX INT16_MAX
#define I32_MIN INT32_MIN
#define I32_MAX INT32_MAX
#define I64_MIN INT64_MIN
#define I64_MAX INT64_MAX

namespace np
{
	using nptr = ::std::nullptr_t;

	using ui8 = uint8_t;
	using ui16 = uint16_t;
	using ui32 = uint32_t;
	using ui64 = uint64_t;

	using i8 = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using siz = size_t;
	using dif = ptrdiff_t;

	using flt = float;
	using dbl = double;
	using ldbl = long double;

	using chr = char;
	using uchr = unsigned char;
	using wchr = wchar_t;
	using chr16 = char16_t;
	using chr32 = char32_t;

	using bl = bool;
} // namespace np

#endif /* NP_ENGINE_PRIMITIVE_TYPES_HPP */
