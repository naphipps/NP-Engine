//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/29/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEMORY_FUNCTIONS_HPP
#define NP_ENGINE_MEMORY_FUNCTIONS_HPP

#include <memory>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::mem
{
	template <class T>
	constexpr T* AddressOf(T& t) noexcept
	{
		return ::std::addressof(t);
	}

	template <class T>
	constexpr T* AddressOf(const T&&) = delete;

	constexpr void CopyBytes(void* dst, const void* src, siz byte_count)
	{
		ui8* dst_it = (ui8*)dst;
		ui8* src_it = (ui8*)src;

		siz loops = byte_count / 8; // we'll copy 8 bytes at a time to start
		for (siz i = 0; i < loops; i++, dst_it += 8, src_it += 8)
			*(ui64*)dst_it = *(ui64*)src_it;

		loops = byte_count % 8; // now we'll copy the remaining bytes to finish
		for (siz i = 0; i < loops; i++, dst_it++, src_it++)
			*dst_it = *src_it;
	}
} // namespace np::mem

#endif /* NP_ENGINE_MEMORY_FUNCTIONS_HPP */
