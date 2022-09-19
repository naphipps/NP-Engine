//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/6/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ACCUMULATING_ALLOCATOR_HPP
#define NP_ENGINE_ACCUMULATING_ALLOCATOR_HPP

#ifndef NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE
	#define NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE (GIGABYTE_SIZE * 2)
#endif

#include <vector>
#include <type_traits>

#include "MemoryFunctions.hpp"
#include "BookkeepingAllocator.hpp"

namespace np::mem
{
	template <typename AllocatorType>
	class AccumulatingAllocator : public Allocator
	{
	private:
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<BookkeepingAllocator, AllocatorType>),
								"AllocatorType must derive from BookkeepingAllocator");

		NP_ENGINE_STATIC_ASSERT(CalcAlignedSize(sizeof(AllocatorType)) < NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE,
								"make NP_ENGINE_APPLICATION_ALLOCATOR_BLOCK_SIZE larger");

		constexpr static siz ALLOCATOR_TYPE_SIZE = CalcAlignedSize(sizeof(AllocatorType));

		CAllocator _c_allocator;
		::std::vector<AllocatorType*> _allocators;

	public:
		~AccumulatingAllocator()
		{
			for (AllocatorType*& allocator : _allocators)
				Destroy(_c_allocator, allocator);

			_allocators.clear();
		}

		bl Contains(const Block& block) const override
		{
			return Contains(block.ptr);
		}

		bl Contains(const void* ptr) const override
		{
			bl contains = false;
			for (siz i = 0; i < _allocators.size() && !contains; i++)
				contains = _allocators[i]->Contains(ptr);

			return contains;
		}

		Block Allocate(siz size) override
		{
			Block b{};
			for (siz i = 0; i < _allocators.size() && !b.IsValid(); i++)
				b = _allocators[i]->Allocate(size);

			if (!b.IsValid())
			{
				siz block_size = NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE;
				while (block_size - ALLOCATOR_TYPE_SIZE - AllocatorType::OVERHEAD_SIZE <= size)
					block_size *= 2;

				Block block = _c_allocator.Allocate(block_size);
				Block allocator_block{block.ptr, ALLOCATOR_TYPE_SIZE};
				Block allocator_given_block{allocator_block.End(), block.size - ALLOCATOR_TYPE_SIZE};
				Construct<AllocatorType>(allocator_block, allocator_given_block);
				_allocators.emplace_back((AllocatorType*)allocator_block.ptr);

				b = _allocators.back()->Allocate(size);
			}

			return b;
		}

		Block ExtractBlock(void* block_ptr) const
		{
			Block block{};
			for (siz i = 0; i < _allocators.size() && !block.IsValid(); i++)
				block = _allocators[i]->ExtractBlock(block_ptr);

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

		Block Reallocate(void* old_ptr, siz new_size) override
		{
			Block old_block = ExtractBlock(old_ptr);
			return Reallocate(old_block, new_size);
		}

		bl Deallocate(Block& block) override
		{
			bl deallocated = Deallocate(block.ptr);
			if (deallocated)
				block.Invalidate();

			return deallocated;
		}

		bl Deallocate(void* ptr) override
		{
			bl deallocated = false;
			for (siz i = 0; i < _allocators.size() && !deallocated; i++)
				if (_allocators[i]->Contains(ptr))
					deallocated = _allocators[i]->Deallocate(ptr);

			return deallocated;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_ACCUMULATING_ALLOCATOR_HPP */