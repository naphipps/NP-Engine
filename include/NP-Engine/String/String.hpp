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

template <class T, class STD_T = ::std::string>
class tstr : public ::std::basic_string<T, ::std::char_traits<T>, ::np::memory::StdAllocator<T>>
{
public:

    using base = ::std::basic_string<T, ::std::char_traits<T>, ::np::memory::StdAllocator<T>>;
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
    tstr(InputIt first, InputIt last) : base(first, last) {}

    operator std_type() const { return base::c_str(); }

    tstr& operator=(const tstr& other) { base::operator=(other);  return *this; }

    tstr& operator=(tstr&& other) noexcept { base::operator=(other);  return *this; }

    tstr& operator+=(const tstr& other) { base::operator+=(other.c_str()); return *this; }
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

template <class T>
static str to_str(T t)
{
    return ::std::to_string(t);
}


using wstr = tstr<wchr, ::std::wstring>;

static wstr to_wstr(const wchr* c)
{
    return wstr(c);
}

template <class T>
static wstr to_wstr(T t)
{
    return ::std::to_wstring(t);
}

//TODO: I'm not quite sure how I want to support u16str and u32str... they may replace wstr since wstr is 16bit on Windows and 32bit elsewhere....
/*
using u16str = tstr<u16chr>;

static u16str to_u16str(const chr* c)
{
    return u16str(c);
}

template <class T>
static u16str to_u16str(T t)
{
    return ::std::to_string(t);
}

using u32str = tstr<u32chr>;

static u32str to_u32str(const chr* c)
{
    return u32str(c);
}

template <class T>
static u32str to_u32str(T t)
{
    return ::std::to_string(t);
}
*/

#endif /* NP_ENGINE_STRING_HPP */