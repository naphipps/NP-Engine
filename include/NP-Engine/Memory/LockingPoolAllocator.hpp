//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 3/6/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_LOCKING_POOL_ALLOCATOR_HPP
#define NP_ENGINE_LOCKING_POOL_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "SizedAllocator.hpp"

namespace np::memory
{
	template <typename T>
	class LockingPoolAllocator : public SizedAllocator
	{
	public:
		constexpr static siz CHUNK_ALIGNED_SIZE = CalcAlignedSize(sizeof(T));

	private:
		Mutex _mutex;
		void* _alloc_iterator;
		bl _deallocation_true_sort_false_constant;

		bl IsChunkPtr(void* ptr) const
		{
			return Contains(ptr) && ((ui8*)ptr - (ui8*)_block.Begin()) % CHUNK_ALIGNED_SIZE == 0;
		}

		void Init()
		{
			Block block{.size = CHUNK_ALIGNED_SIZE};

			for (ui32 i = 0; i < ChunkCount() - 1; i++)
			{
				block.ptr = &static_cast<ui8*>(_block.ptr)[i * CHUNK_ALIGNED_SIZE];
				Construct<void*>(block, &static_cast<ui8*>(_block.ptr)[(i + 1) * CHUNK_ALIGNED_SIZE]);
			}

			block.ptr = &static_cast<ui8*>(_block.ptr)[(ChunkCount() - 1) * CHUNK_ALIGNED_SIZE];
			Construct<void*>(block, nullptr);

			_alloc_iterator = _block.ptr;
		}

	public:
		LockingPoolAllocator(Block block): SizedAllocator(block), _deallocation_true_sort_false_constant(false)
		{
			Init();
		}

		LockingPoolAllocator(siz size): SizedAllocator(size), _deallocation_true_sort_false_constant(false)
		{
			Init();
		}

		siz ChunkCount() const
		{
			return _block.size / CHUNK_ALIGNED_SIZE;
		}

		siz ChunkSize() const
		{
			return CHUNK_ALIGNED_SIZE;
		}

		bl GetDeallocationReclaimBehavior() const
		{
			return _deallocation_true_sort_false_constant;
		}

		void SetDeallocationReclaimBehavior(bl true_sort_false_constant = false)
		{
			_deallocation_true_sort_false_constant = true_sort_false_constant;
		}

		void Zeroize() override
		{
			Lock lock(_mutex);
			SizedAllocator::Zeroize();
			Init();
		}

		Block Allocate(siz size) override
		{
			Lock lock(_mutex);
			Block block;

			if (size <= CHUNK_ALIGNED_SIZE && _alloc_iterator != nullptr)
			{
				block.ptr = _alloc_iterator;
				block.size = CHUNK_ALIGNED_SIZE;
				_alloc_iterator = *(void**)(&static_cast<ui8*>(_alloc_iterator)[0]);
			}

			return block;
		}

		bl Deallocate(Block& block, bl true_sort_false_constant)
		{
			Lock lock(_mutex);
			bl deallocated = false;

			if (IsChunkPtr(block.ptr) && block.size == CHUNK_ALIGNED_SIZE)
			{
				void** deallocation_address = nullptr;

				if (true_sort_false_constant && _alloc_iterator != nullptr)
				{
					for (void** it = (void**)_alloc_iterator; Contains(it); it = (void**)*it)
					{
						deallocation_address = it;

						if (*it > block.ptr)
						{
							break;
						}
					}

					NP_ASSERT(deallocation_address != nullptr, "our sorted insert failed");
				}
				else
				{
					deallocation_address = &_alloc_iterator;
				}

				Construct<void*>(block, *deallocation_address);
				*deallocation_address = block.ptr;
				block.Invalidate();
				deallocated = true;
			}

			return deallocated;
		}

		bl Deallocate(Block& block) override
		{
			return Deallocate(block, _deallocation_true_sort_false_constant);
		}

		bl Deallocate(void* ptr, bl true_sort_false_constant)
		{
			Block dealloc_block{.ptr = ptr, .size = CHUNK_ALIGNED_SIZE};
			return Deallocate(dealloc_block, true_sort_false_constant);
		}

		bl Deallocate(void* ptr) override
		{
			Block dealloc_block{.ptr = ptr, .size = CHUNK_ALIGNED_SIZE};
			return Deallocate(dealloc_block);
		}

		bl DeallocateAll() override
		{
			Lock lock(_mutex);
			Init();
			return true;
		}
	};
} // namespace np::memory

#endif /* NP_ENGINE_LOCKING_POOL_ALLOCATOR_HPP */
