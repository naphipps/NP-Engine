//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_STACK_ALLOCATOR_HPP
#define NP_ENGINE_STACK_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "SizedAllocator.hpp"
#include "Block.hpp"

namespace np::memory
{
	class LinearAllocator : public SizedAllocator
	{
	private:
		atm<void*> _alloc_iterator;

	public:
		LinearAllocator(Block block): SizedAllocator(block), _alloc_iterator(_block.ptr) {}

		LinearAllocator(siz size): SizedAllocator(size), _alloc_iterator(_block.ptr) {}

		void Zeroize() override
		{
			_alloc_iterator.store(nullptr, mo_release);
			SizedAllocator::Zeroize();
			_alloc_iterator.store(_block.ptr, mo_release);
		}

		Block Allocate(siz size) override
		{
			void* allocated = _alloc_iterator.load(mo_acquire);
			size = CalcAlignedSize(size);

			while (
				allocated && static_cast<ui8*>(allocated) + size <= _block.End() &&
				!_alloc_iterator.compare_exchange_weak(allocated, static_cast<ui8*>(allocated) + size, mo_release, mo_relaxed))
				;

			Block block;

			if (allocated && static_cast<ui8*>(allocated) + size <= _block.End())
			{
				block.ptr = allocated;
				block.size = size;
			}

			return block;
		}

		bl Deallocate(Block& block) override
		{
			return false;
		}

		bl Deallocate(void* ptr) override
		{
			return false;
		}

		bl DeallocateAll() override
		{
			_alloc_iterator.store(_block.ptr, mo_release);
			return true;
		}
	};
} // namespace np::memory

#endif /* NP_ENGINE_STACK_ALLOCATOR_HPP */
