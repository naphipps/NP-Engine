//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/12/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_STACK_ALLOCATOR_HPP
#define NP_ENGINE_STACK_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"
#include "NP-Engine/Foundation/Foundation.hpp"

#include "BlockedAllocator.hpp"
#include "Block.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class LinearAllocator : public BlockedAllocator
	{
	private:
		atm<void*> _allocation;

		Block Reallocate(void* old_ptr, siz new_size) override
		{
			return {};
		}

		bl Deallocate(Block& block) override
		{
			return false;
		}

		bl Deallocate(void* ptr) override
		{
			return false;
		}

	public:
		LinearAllocator(Block block): BlockedAllocator(block), _allocation(_block.ptr) {}

		LinearAllocator(siz size): BlockedAllocator(size), _allocation(_block.ptr) {}

		void Zeroize() override
		{
			_allocation.store(nullptr, mo_release);
			BlockedAllocator::Zeroize();
			_allocation.store(_block.ptr, mo_release);
		}

		Block Allocate(siz size) override
		{
			void* allocated = _allocation.load(mo_acquire);
			size = CalcAlignedSize(size);

			while (allocated && static_cast<ui8*>(allocated) + size <= _block.End() &&
				   !_allocation.compare_exchange_weak(allocated, static_cast<ui8*>(allocated) + size, mo_release, mo_relaxed))
			{}

			Block block;

			if (allocated && static_cast<ui8*>(allocated) + size <= _block.End())
			{
				block.ptr = allocated;
				block.size = size;
			}

			return block;
		}

		Block Reallocate(Block& old_block, siz new_size) override
		{
			Block new_block = Allocate(new_size);
			if (Contains(old_block))
			{
				CopyBytes(new_block.Begin(), old_block.Begin(), old_block.size);
				Deallocate(old_block);
				old_block.Invalidate();
			}

			return new_block;
		}

		bl DeallocateAll() override
		{
			_allocation.store(_block.ptr, mo_release);
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_STACK_ALLOCATOR_HPP */
