//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 2/15/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_POOL_ALLOCATOR_HPP
#define NP_ENGINE_POOL_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "BlockedAllocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	template <typename T>
	class PoolAllocator : public BlockedAllocator
	{
	public:
		constexpr static siz CHUNK_SIZE = CalcAlignedSize(sizeof(T));

	private:
		atm<void*> _allocation;

		bl IsChunkPtr(void* ptr)
		{
			return Contains(ptr) && ((ui8*)ptr - (ui8*)_block.Begin()) % CHUNK_SIZE == 0;
		}

		void Init()
		{
			Block block{nullptr, CHUNK_SIZE};
			for (siz i = 0; i < ChunkCount() - 1; i++)
			{
				block.ptr = &static_cast<ui8*>(_block.ptr)[i * CHUNK_SIZE];
				mem::Construct<void*>(block, &static_cast<ui8*>(_block.ptr)[(i + 1) * CHUNK_SIZE]);
			}

			block.ptr = &static_cast<ui8*>(_block.ptr)[(ChunkCount() - 1) * CHUNK_SIZE];
			mem::Construct<void*>(block, nullptr);

			_allocation.store(_block.ptr, mo_release);
		}

		Block Reallocate(void* old_ptr, siz new_size) override
		{
			return {};
		}

	public:
		PoolAllocator(Block block): BlockedAllocator(block)
		{
			Init();
		}

		PoolAllocator(siz size): BlockedAllocator(size)
		{
			Init();
		}

		siz ChunkCount() const
		{
			return _block.size / CHUNK_SIZE;
		}

		siz ChunkSize() const
		{
			return CHUNK_SIZE;
		}

		void Zeroize() override
		{
			_allocation.store(nullptr, mo_release);
			BlockedAllocator::Zeroize();
			Init();
		}

		Block Allocate(siz size) override
		{
			Block block;
			if (size <= CHUNK_SIZE)
			{
				void* allocated = _allocation.load(mo_acquire);

				while (allocated &&
					   !_allocation.compare_exchange_weak(allocated, *(void**)(&static_cast<ui8*>(allocated)[0]), mo_release,
														  mo_relaxed))
				{}

				if (allocated)
				{
					block.ptr = allocated;
					block.size = CHUNK_SIZE;
				}
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

		bl Deallocate(Block& block) override
		{
			bl deallocated = false;
			if (IsChunkPtr(block.ptr) && block.size == CHUNK_SIZE)
			{
				mem::Construct<void*>(block, _allocation.load(mo_acquire));

				while (!_allocation.compare_exchange_weak(*(void**)(&static_cast<ui8*>(block.ptr)[0]), block.ptr, mo_release,
														  mo_relaxed))
				{}

				block.Invalidate();
				deallocated = true;
			}

			return deallocated;
		}

		bl Deallocate(void* ptr) override
		{
			Block dealloc_block{ptr, CHUNK_SIZE};
			return Deallocate(dealloc_block);
		}

		bl DeallocateAll() override
		{
			_allocation.store(nullptr, mo_release);
			Init();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_POOL_ALLOCATOR_HPP */
