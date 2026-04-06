//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 12/20/25
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_ALIGNMENT_HPP
#define NP_ENGINE_MEM_ALIGNMENT_HPP

#include <memory>

#include "NP-Engine/Primitive/Primitive.hpp"

namespace np::mem
{
	namespace __detail
	{
		constexpr static siz DEFAULT_ALIGNMENT_BIT = 3;
	}

	/*
		the alignment our allocators will default to
	*/
	constexpr static siz DEFAULT_ALIGNMENT = BIT(__detail::DEFAULT_ALIGNMENT_BIT);
	constexpr static siz DEFAULT_ALIGNMENT_MASK = (DEFAULT_ALIGNMENT - 1);

	/*
		gets the next power of 2 if not already
		minimum is DEFAULT_ALIGNMENT
	*/
	constexpr static siz sanitize_alignment(siz alignment)
	{
		for (ui64 i = __detail::DEFAULT_ALIGNMENT_BIT; i < SIZ_BIT_COUNT; i++)
			if (BIT(i) >= alignment)
			{
				alignment = BIT(i);
				break;
			}

		return alignment;
	}

	/*
		true iff value is a multiple of alignment, else false
		calls sanitize_alignment on alignment
	*/
	constexpr static bl is_aligned(siz value, siz alignment)
	{
		alignment = sanitize_alignment(alignment);
		return (value % alignment) == 0;
	}

	/*
		true iff ptr_ is a multiple of alignment, else false
		calls sanitize_alignment on alignment
	*/
	constexpr static bl is_aligned(void* ptr_, siz alignment)
	{
		NP_ENGINE_STATIC_ASSERT(sizeof(void*) == sizeof(siz), "size of void* must equal size of siz");

		alignment = sanitize_alignment(alignment);

		union {
			void* ptr = nullptr;
			siz value;
		};

		ptr = ptr_;
		return is_aligned(value, alignment);
	}

	/*
		calls sanitize_alignment on alignment
	*/
	constexpr static siz calc_aligned_value(siz value, siz alignment)
	{
		alignment = sanitize_alignment(alignment);
		return ((value + alignment - 1) / alignment) * alignment;
	}

	constexpr static siz calc_aligned_size(siz size, siz alignment)
	{
		return calc_aligned_value(size, alignment);
	}

	/*
		calls sanitize_alignment on alignment
		returns next aligned ptr if not already
	*/
	constexpr static void* calc_aligned_ptr(void* ptr_, siz alignment)
	{
		NP_ENGINE_STATIC_ASSERT(sizeof(void*) == sizeof(siz), "size of void* must equal size of siz");

		alignment = sanitize_alignment(alignment);

		union {
			void* ptr = nullptr;
			siz value;
		};

		ptr = ptr_;
		value = calc_aligned_value(value, alignment);
		return value == 0 ? nullptr : ptr;
	}
} // namespace np::mem

#endif /* NP_ENGINE_MEM_ALIGNMENT_HPP */
