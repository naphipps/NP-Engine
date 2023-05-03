//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/6/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ACCUMULATING_ALLOCATOR_HPP
#define NP_ENGINE_ACCUMULATING_ALLOCATOR_HPP

#ifndef NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE
	#define NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE GIGABYTE_SIZE
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

		NP_ENGINE_STATIC_ASSERT((CalcAlignedSize(sizeof(AllocatorType)) < NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE),
								"make NP_ENGINE_APPLICATION_ALLOCATOR_BLOCK_SIZE larger");

		constexpr static siz ALLOCATOR_TYPE_SIZE = CalcAlignedSize(sizeof(AllocatorType));

		CAllocator _c_allocator;
		mutexed_wrapper<::std::vector<AllocatorType*>> _allocators;

	public:
		~AccumulatingAllocator()
		{
			DeallocateAll();
		}

		bl Contains(const Block& block) override
		{
			return Contains(block.ptr);
		}

		bl Contains(const void* ptr) override
		{
			bl contains = false;
			auto allocators = _allocators.get_access();
			for (siz i = 0; i < allocators->size() && !contains; i++)
				contains = (*allocators)[i]->Contains(ptr);
			return contains;
		}

		Block Allocate(siz size) override
		{
			Block b{};
			auto allocators = _allocators.get_access();
			for (siz i = 0; i < allocators->size() && !b.IsValid(); i++)
				b = (*allocators)[i]->Allocate(size);

			if (!b.IsValid())
			{
				siz block_size = NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE;
				while (block_size - ALLOCATOR_TYPE_SIZE - AllocatorType::OVERHEAD_SIZE <= size)
					block_size *= 2;

				Block block = _c_allocator.Allocate(block_size);
				Block object_block{block.ptr, ALLOCATOR_TYPE_SIZE};
				Block allocate_block{ object_block.End(), block.size - object_block.size};
				allocators->emplace_back(mem::Construct<AllocatorType>(object_block, allocate_block));
				b = allocators->back()->Allocate(size);
			}

			return b;
		}

		Block ExtractBlock(void* block_ptr)
		{
			Block block{};
			auto allocators = _allocators.get_access();
			for (siz i = 0; i < allocators->size() && !block.IsValid(); i++)
				block = (*allocators)[i]->ExtractBlock(block_ptr);
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
			auto allocators = _allocators.get_access();
			for (siz i = 0; i < allocators->size() && !deallocated; i++)
				if ((*allocators)[i]->Contains(ptr))
					deallocated = (*allocators)[i]->Deallocate(ptr);
			return deallocated;
		}

		bl DeallocateAll()
		{
			auto allocators = _allocators.get_access();
			for (AllocatorType* allocator : *allocators)
				Destroy<AllocatorType>(_c_allocator, allocator);
			allocators->clear();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_ACCUMULATING_ALLOCATOR_HPP */