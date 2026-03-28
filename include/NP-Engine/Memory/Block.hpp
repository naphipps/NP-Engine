//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_BLOCK_HPP
#define NP_ENGINE_MEM_BLOCK_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Alignment.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	/*
		our Block design was inspired by Andrei Alexandrescu's cppcon 2017 talk "std::allocator is to allocation what
		std::vector is to vexation" <https://www.youtube.com/watch?v=LIb3L4vKZ7U>
	*/
	struct block
	{
		void* ptr = nullptr;
		siz size = 0;

		void invalidate()
		{
			ptr = nullptr;
			size = 0;
		}

		bl empty() const
		{
			return size == 0;
		}

		bl is_valid() const
		{
			return ptr != nullptr && !empty();
		}

		void* begin() const
		{
			return is_valid() ? ptr : nullptr;
		}

		void* end() const
		{
			return is_valid() ? static_cast<ui8*>(ptr) + size : nullptr;
		}

		bl contains(const void* p) const
		{
			return is_valid() ? ptr <= p && p < end() : false;
		}

		bl contains(const block& other) const
		{
			return is_valid() && other.is_valid() && contains(other.begin()) && contains(static_cast<ui8*>(other.end()) - 1);
		}

		void zeroize() const
		{
			mem::zeroize(ptr, size);
		}

		bl is_zeroized() const
		{
			return mem::is_zeroized(ptr, size);
		}

		/*
			returns the block representing the offset so it's end() is aligned to the DEFAULT_ALIGNMENT

			the user should always uses the values allocators produce, but they use allocators with rogue values,
			thus we unforntuately have to assume DEFAULT_ALIGNMENT
		*/
		block get_offset_block(void* ptr_) const
		{
			return is_valid() && contains(ptr_) && is_aligned(ptr_, DEFAULT_ALIGNMENT) ?
				block{ ptr, (siz)(static_cast<ui8*>(ptr_) - static_cast<ui8*>(begin())) } : block{};
		}

		/*
			returns this block unless offset is valid, contained within this, and offset.begin() equals our begin()
		*/
		block get_remaining_block(block offset) const
		{
			return is_valid() && offset.is_valid() && contains(offset) && offset.begin() == begin() ?
				block{ offset.end(), size - offset.size } : *this;
		}

		/*
			returns the pair<offset, remaining (aka: aligned)> of blocks, split on given alignment
			calls sanitize_alignment on alignment
		*/
		::std::pair<block, block> get_split_on_alignment(siz alignment) const
		{
			alignment = sanitize_alignment(alignment);
			void* aligned_ptr = calc_aligned_ptr(ptr, alignment);
			block offset = is_valid() ? get_offset_block(aligned_ptr) : block{};
			block remaining = get_remaining_block(offset); //remaining is aligned
			return { offset, remaining };
		}
	};

	template <siz SIZE_, siz ALIGNMENT_>
	struct sized_block
	{
		NP_ENGINE_STATIC_ASSERT(SIZE_ > 0, "(S)ize must be greater than zero.");

		constexpr static siz ALIGNMENT = sanitize_alignment(ALIGNMENT_);
		constexpr static siz SIZE = calc_aligned_size(SIZE_, ALIGNMENT);

		ui8 allocation[SIZE];

		operator block() const
		{
			return { (void*)allocation, SIZE };
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_BLOCK_HPP */
