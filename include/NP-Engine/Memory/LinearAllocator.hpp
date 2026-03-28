//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_LINEAR_ALLOCATOR_HPP
#define NP_ENGINE_MEM_LINEAR_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Foundation/Foundation.hpp"

#include "BlockedAllocator.hpp"
#include "Block.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class linear_allocator : public blocked_allocator
	{
	protected:
		atm<void*> _allocation;

	public:
		linear_allocator(block b): blocked_allocator(b), _allocation(_block.ptr) {}

		linear_allocator(siz size, siz alignment): blocked_allocator(size, alignment), _allocation(_block.ptr) {}

		virtual ~linear_allocator() = default;

		virtual block allocate(siz size, siz alignment) override
		{
			block b{ _allocation.load(mo_acquire), calc_aligned_size(size, alignment) };

			while (contains(b) && !_allocation.compare_exchange_weak(b.ptr, b.end(), mo_release, mo_relaxed))
			{}

			if (b.is_valid())
			{
				::std::pair<block, block> split = b.get_split_on_alignment(alignment);
				b = split.second;
			}

			return contains(b) ? b : block{};
		}

		virtual block reallocate(block& b_, siz size, siz alignment) override
		{
			block b = allocate(size, alignment);
			if (contains(b_))
			{
				const siz byte_count = b.size < b_.size ? b.size : b_.size;
				copy_bytes(b.begin(), b_.begin(), byte_count);
				deallocate(b_);
				b_.invalidate();
			}
			return b;
		}

		/*
			this value is meaningless from linear_allocator
		*/
		virtual block reallocate(void* ptr, siz size, siz alignment) override
		{
			return {};
		}

		/*
			this value is meaningless from linear_allocator
		*/
		virtual bl deallocate(block& b) override
		{
			return false;
		}

		/*
			this value is meaningless from linear_allocator
		*/
		virtual bl deallocate(void* ptr) override
		{
			return false;
		}

		virtual bl deallocate_all() override
		{
			_allocation.store(_block.ptr, mo_release);
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_LINEAR_ALLOCATOR_HPP */
