//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 11/15/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_STRING_HPP
#define NP_ENGINE_STRING_HPP

#include <string>
#include <cstring>
#include <initializer_list>

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Memory/Memory.hpp"

namespace np
{

	template <class T, class STD_T = ::std::string>
	class tstr : public ::std::basic_string<T, ::std::char_traits<T>, ::np::mem::StdAllocator<T>>
	{
	public:
		using base = ::std::basic_string<T, ::std::char_traits<T>, ::np::mem::StdAllocator<T>>;
		using traits_type = typename base::traits_type;
		using allocator_type = typename base::allocator_type;
		using value_type = typename base::value_type;
		using size_type = typename base::size_type;
		using difference_type = typename base::difference_type;
		using pointer = typename base::pointer;
		using const_pointer = typename base::const_pointer;
		using reference = typename base::reference;
		using const_reference = typename base::const_reference;
		using iterator = typename base::iterator;
		using const_iterator = typename base::const_iterator;
		using reverse_iterator = typename base::reverse_iterator;
		using const_reverse_iterator = typename base::const_reverse_iterator;
		using std_type = STD_T;

		tstr() noexcept : base() {}

		tstr(size_type count, T c) : base(count, c) {}

		tstr(const base& b) : base(b.c_str()) {}

		tstr(const tstr& other) : base(other) {}

		tstr(tstr&& other) noexcept : base(other) {}

		tstr(const T* chrs) : base(chrs) {}

		tstr(const T* chrs, size_type count) : base(chrs, count) {}

		tstr(const std_type& other) : tstr<T>(other.c_str()) {}

		tstr(::std::initializer_list<T> list) : base(list) {}

		template <class InputIt>
		tstr(InputIt first, InputIt last) : base(first, last)
		{}

		operator std_type() const
		{
			return base::c_str();
		}

		tstr& operator=(const tstr& other)
		{
			base::operator=(other);
			return *this;
		}

		tstr& operator=(tstr&& other) noexcept
		{
			base::operator=(other);
			return *this;
		}

		tstr& operator+=(const tstr& other)
		{
			base::operator+=(other.c_str());
			return *this;
		}
	};

	template <class T>
	tstr<T> operator+(const tstr<T>& left, const tstr<T>& right)
	{
		return tstr<T>(left) += right;
	}

	template <class T, class U>
	tstr<T> operator+(U left, const tstr<T>& right)
	{
		return tstr<T>(left) += right;
	}

	template <class T, class U>
	tstr<T> operator+(const tstr<T>& left, U right)
	{
		return tstr<T>(left) += right;
	}

	using str = tstr<chr>;

	static str to_str(const chr* c)
	{
		return str(c);
	}

	static str to_str(chr* c)
	{
		return str(c);
	}

	template <class T>
	static str to_str(T t)
	{
		return ::std::to_string(t);
	}
}

namespace std
{
	template <>
	struct hash<::np::str>
	{
		::np::siz operator()(const ::np::str& s) const noexcept
		{
			return ::std::hash<::np::str::std_type>{}((::np::str::std_type)s);
		}
	};
} // namespace std

namespace np
{
	using wstr = tstr<wchr, ::std::wstring>; //TODO: this is 16bit on windows, 32bit elsewhere... our wstr should be 32bit

	static wstr to_wstr(const wchr* c)
	{
		return wstr(c);
	}

	static wstr to_wstr(wchr* c)
	{
		return wstr(c);
	}

	template <class T>
	static wstr to_wstr(T t)
	{
		return ::std::to_wstring(t);
	}

	/*
	using str16 = tstr<chr16>;

	static str16 to_str16(const chr* c)
	{
		return str16(c);
	}

	static str16 to_str16(chr* c)
	{
		return str16(c);
	}

	template <class T>
	static str16 to_str16(T t)
	{
		return ::std::to_wstring(t);
	}

	using str32 = tstr<chr32>;

	static str32 to_str32(const chr* c)
	{
		return str32(c);
	}

	static str32 to_str32(chr* c)
	{
		return str32(c);
	}

	template <class T>
	static str32 to_str32(T t)
	{
		return ::std::to_wstring(t);
	}
	//*/
}

namespace std
{
	template <>
	struct hash<::np::wstr>
	{
		::np::siz operator()(const ::np::wstr& s) const noexcept
		{
			return ::std::hash<::np::wstr::std_type>{}((::np::wstr::std_type)s);
		}
	};
} // namespace std

#endif /* NP_ENGINE_STRING_HPP */