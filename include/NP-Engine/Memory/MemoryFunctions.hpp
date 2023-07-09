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
	/*
		the alignment our allocators will adhere to
	*/
	constexpr static siz ALIGNMENT = BIT(3);
	constexpr static siz ALIGNMENT_MINUS_ONE = (ALIGNMENT - 1);

	constexpr static siz CalcAlignedSize(const siz size)
	{
		return ((size + ALIGNMENT - 1) / ALIGNMENT) * ALIGNMENT;
	}

	template <class T>
	constexpr T* AddressOf(T& t) noexcept
	{
		return ::std::addressof(t);
	}

	constexpr void CopyBytes(void* dst, const void* src, siz byte_count)
	{
		ui8* dst_it = (ui8*)dst;
		ui8* src_it = (ui8*)src;
		const siz stride = sizeof(siz);

		siz loops = byte_count / stride; // we'll copy 8 bytes at a time to start
		for (siz i = 0; i < loops; i++, dst_it += stride, src_it += stride)
			*(siz*)dst_it = *(siz*)src_it;

		loops = byte_count % stride; // now we'll copy the remaining bytes to finish
		for (siz i = 0; i < loops; i++, dst_it++, src_it++)
			*dst_it = *src_it;
	}
} // namespace np::mem

#endif /* NP_ENGINE_MEMORY_FUNCTIONS_HPP */
