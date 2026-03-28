//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/29/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_MEMORY_FUNCTIONS_HPP
#define NP_ENGINE_MEM_MEMORY_FUNCTIONS_HPP

#include <memory>
#include <utility>
#include <type_traits>
#include <tuple>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::mem
{
	namespace __detail
	{
		//<https://stackoverflow.com/questions/47343146/how-can-i-detect-whether-a-type-is-list-initializable>

		template <typename T, typename Args, typename = void>
		struct is_list_constructible_impl : ::std::false_type {};

		template <typename T, typename... Args>
		struct is_list_constructible_impl < T, ::std::tuple<Args...>, ::std::void_t<decltype(T{ ::std::declval<Args>()... }) >> : ::std::true_type{};
	}

	template <typename T, typename... Args>
	using is_list_constructible = __detail::is_list_constructible_impl<T, ::std::tuple<Args...>>;

	template <typename T, typename... Args>
	constexpr bl is_list_constructible_v = is_list_constructible<T, Args...>::value;

	template <typename T, typename... Args>
	using is_parenthesis_constructible = ::std::is_constructible<T, Args...>;

	template <typename T, typename... Args>
	constexpr bl is_parenthesis_constructible_v = is_parenthesis_constructible<T, Args...>::value;

	template <typename T>
	constexpr T* address_of(T& t) noexcept
	{
		return ::std::addressof(t);
	}

	constexpr void zeroize(void* dst, siz byte_count)
	{
		ui8* it = static_cast<ui8*>(dst);
		const siz stride = sizeof(siz);

		siz loops = byte_count / stride; // we'll zeroize 8 bytes at a time to start
		for (siz i = 0; i < loops; i++, it += stride)
			*(siz*)it = 0;

		loops = byte_count % stride; // now we'll zeroize the remaining bytes to finish
		for (siz i = 0; i < loops; i++, it++)
			*it = 0;
	}

	constexpr bl is_zeroized(void* ptr, siz byte_count)
	{
		bl is = false;
		ui8* it = static_cast<ui8*>(ptr);
		const siz stride = sizeof(siz);

		siz loops = byte_count / stride; // check 8 bytes at a time to start
		for (siz i = 0; !is && i < loops; i++, it += stride)
			is |= *(siz*)it == 0;

		loops = byte_count % stride; // now check the remaining bytes to finish
		for (siz i = 0; !is && i < loops; i++, it++)
			is |= *it == 0;

		return is;
	}

	constexpr void copy_bytes(void* dst, const void* src, siz byte_count)
	{
		ui8* dst_it = static_cast<ui8*>(dst);
		ui8* src_it = static_cast<ui8*>(const_cast<void*>(src));
		const siz stride = sizeof(siz);

		siz loops = byte_count / stride; // we'll copy 8 bytes at a time to start
		for (siz i = 0; i < loops; i++, dst_it += stride, src_it += stride)
			*(siz*)dst_it = *(siz*)src_it;

		loops = byte_count % stride; // now we'll copy the remaining bytes to finish
		for (siz i = 0; i < loops; i++, dst_it++, src_it++)
			*dst_it = *src_it;
	}

	/*
		recommended: dst != src, otherwise you'll get what you get
	*/
	constexpr void reverse_bytes(void* dst, const void* src, siz byte_count)
	{
		ui8* dst_it = static_cast<ui8*>(dst) + byte_count - 1;
		ui8* src_it = static_cast<ui8*>(const_cast<void*>(src));
		for (siz i = 0; i < byte_count; i++, dst_it--, src_it++)
			*dst_it = *src_it;
	}
} // namespace np::mem

#endif /* NP_ENGINE_MEM_MEMORY_FUNCTIONS_HPP */
