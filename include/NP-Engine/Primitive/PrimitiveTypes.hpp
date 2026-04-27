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

//FLT_MIN and FLT_MAX already defined
//DBL_MIN and DBL_MAX already defined

#define CHR_MIN CHAR_MIN
#define CHR_MAX CHAR_MAX
#define UCHR_MIN 0
#define UCHR_MAX UCHAR_MAX
#define WCHR_MIN WCHAR_MIN
#define WCHR_MAX WCHAR_MAX

//TODO: add min/max for: chr16, chr32

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

	/*
		TODO: when updating to cpp23, float16_t is available in include <stdfloat>, so remove flt, dbl, and ldbl in favor of the f16, f32, f64 (or flt16, flt32, flt64)
			#include <stdfloat>
			using f16 = float16_t;
			using f32 = float32_t;
			using f64 = float64_t;

		TODO: investigate supporting fixed point decimal values somehow -- seems to be a million ways
	*/

	using flt = float;
	using dbl = double;
	using ldbl = long double;

	using chr = char;
	using uchr = unsigned char;
	using wchr = wchar_t;
	using chr16 = char16_t;
	using chr32 = char32_t;

	using bl = bool;

	template <typename T>
	class enm 
	{
	protected:
		T _value;

		void set_embedded_value(T value, T mask, T shift)
		{
			value <<= shift;
			value &= mask;
			_value &= ~mask;
			_value |= value;
		}

		T get_embedded_value(T mask, T shift) const
		{
			return (_value & mask) >> shift;
		}

	public:
		constexpr static T zero = 0;
		constexpr static T none = zero;
		constexpr static T empty = zero;
		constexpr static T nothing = zero;
		constexpr static T all = ~((T)0);
		constexpr static T full = all;
		constexpr static T everything = all;

		//aliases since we eventually like to use pascal casing
		constexpr static T Zero = zero;
		constexpr static T None = none;
		constexpr static T Empty = empty;
		constexpr static T Nothing = nothing;
		constexpr static T All = all;
		constexpr static T Full = full;
		constexpr static T Everything = everything;

		static inline bl contains(const enm<T>& a, const enm<T>& b)
		{
			return (a._value & b._value) == b._value;
		}

		static inline bl contains_any(const enm<T>& a, const enm<T>& b)
		{
			return (a._value & b._value) != None;
		}

		static inline bl equals(const enm<T>& a, const enm<T>& b)
		{
			return a._value == b._value;
		}

		//aliases since we eventually like to use pascal casing
		static inline bl Contains(const enm<T>& a, const enm<T>& b)
		{
			return contains(a, b);
		}

		static inline bl ContainsAny(const enm<T>& a, const enm<T>& b)
		{
			return contains_any(a, b);
		}

		static inline bl Equals(const enm<T>& a, const enm<T>& b)
		{
			return equals(a, b);
		}

		enm(T value): _value(value) {}

		operator T() const
		{
			return _value;
		}

		enm<T>& operator|=(const enm<T>& other)
		{
			_value |= other._value;
			return *this;
		}

		enm<T>& operator&=(const enm<T>& other)
		{
			_value &= other._value;
			return *this;
		}

		bl contains(const enm<T>& other) const
		{
			return contains(*this, other);
		}

		bl contains_any(const enm<T>& other) const
		{
			return contains_any(*this, other);
		}

		bl equals(const enm<T>& other) const
		{
			return equals(*this, other);
		}

		//aliases since we eventually like to use pascal casing
		bl Contains(const enm<T>& other) const
		{
			return contains(other);
		}

		bl ContainsAny(const enm<T>& other) const
		{
			return contains_any(other);
		}

		bl Equals(const enm<T>& other) const
		{
			return equals(other);
		}
	};

	using enm_ui8 = enm<ui8>;
	using enm_ui16 = enm<ui16>;
	using enm_ui32 = enm<ui32>;
	using enm_ui64 = enm<ui64>;
	using enm_siz = enm<siz>;
} // namespace np

#endif /* NP_ENGINE_PRIMITIVE_TYPES_HPP */
