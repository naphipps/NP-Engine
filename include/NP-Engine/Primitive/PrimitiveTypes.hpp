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

	template <typename T>
	class Enum
	{
	protected:
		T _value;

		virtual void SetEmbeddedValue(T value, T mask, T shift)
		{
			value <<= shift;
			value &= mask;
			_value &= ~mask;
			_value |= value;
		}

		virtual T GetEmbeddedValue(T mask, T shift) const
		{
			return (_value & mask) >> shift;
		}

	public:
		constexpr static T Zero = 0;
		constexpr static T None = Zero;
		constexpr static T Empty = Zero;
		constexpr static T Nothing = Zero;
		constexpr static T All = ~((T)0);
		constexpr static T Full = All;
		constexpr static T Everything = All;

		static inline bl Contains(const Enum<T>& a, const Enum<T>& b)
		{
			return (a._value & b._value) == b._value;
		}

		static inline bl ContainsAny(const Enum<T>& a, const Enum<T>& b)
		{
			return (a._value & b._value) != None;
		}

		static inline bl ContainsAll(const Enum<T>& a, const Enum<T>& b)
		{
			return a._value == b._value;
		}

		Enum(T value) : _value(value) {}

		virtual ~Enum() = default;

		operator T() const
		{
			return _value;
		}

		Enum<T>& operator |=(const Enum<T>& other)
		{
			_value |= other._value;
			return *this;
		}

		Enum<T>& operator &=(const Enum<T>& other)
		{
			_value &= other._value;
			return *this;
		}

		virtual bl Contains(const Enum<T>& other) const
		{
			return Contains(*this, other);
		}

		virtual bl ContainsAny(const Enum<T>& other) const
		{
			return ContainsAny(*this, other);
		}

		virtual bl ContainsAll(const Enum<T>& other) const
		{
			return ContainsAll(*this, other);
		}
	};
} // namespace np

#endif /* NP_ENGINE_PRIMITIVE_TYPES_HPP */
