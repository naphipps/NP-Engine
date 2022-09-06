//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/6/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_ACCUMULATING_ALLOCATOR_HPP
#define NP_ENGINE_ACCUMULATING_ALLOCATOR_HPP

#include <vector>

#include "NP-Engine/Memory/Memory.hpp"

#ifndef NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE
	#define NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE (::np::mem::GIGABYTE_SIZE * 2)
#endif

namespace np::app
{
	class AccumulatingAllocator : public mem::Allocator
	{
	private:
		NP_ENGINE_STATIC_ASSERT(mem::CalcAlignedSize(sizeof(mem::RedBlackTreeAllocator)) < 
			NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE,
			"make NP_ENGINE_APPLICATION_ALLOCATOR_BLOCK_SIZE larger");

		constexpr static siz RBT_ALIGNED_SIZE = mem::CalcAlignedSize(sizeof(mem::RedBlackTreeAllocator));

		mem::CAllocator _allocator;
		::std::vector<mem::RedBlackTreeAllocator*> _rbt_ptrs;

	public:
		~AccumulatingAllocator()
		{
			for (mem::RedBlackTreeAllocator*& rbt_ptr : _rbt_ptrs)
				mem::Destroy(_allocator, rbt_ptr);

			_rbt_ptrs.clear();
		}

		bl Contains(const mem::Block& block) const override
		{
			return Contains(block.ptr);
		}

		bl Contains(const void* ptr) const override
		{
			bl contains = false;
			for (siz i = 0; i < _rbt_ptrs.size() && !contains; i++)
				contains = _rbt_ptrs[i]->Contains(ptr);

			return contains;
		}

		mem::Block Allocate(siz size) override
		{
			mem::Block b{};

			for (siz i = 0; i < _rbt_ptrs.size() && !b.IsValid(); i++)
				b = _rbt_ptrs[i]->Allocate(size);

			if (!b.IsValid())
			{
				siz block_size = NP_ENGINE_ACCUMULATING_ALLOCATOR_BLOCK_SIZE;
				while (block_size - RBT_ALIGNED_SIZE <= size)
					block_size *= 2;

				mem::Block block = _allocator.Allocate(block_size);
				mem::Block rbt_block{ block.ptr, RBT_ALIGNED_SIZE};
				mem::Block rbt_alloc_block{ rbt_block.End(), block.size - RBT_ALIGNED_SIZE };
				mem::Construct<mem::RedBlackTreeAllocator>(rbt_block, rbt_alloc_block);
				_rbt_ptrs.emplace_back((mem::RedBlackTreeAllocator*)rbt_block.ptr);

				b = _rbt_ptrs.back()->Allocate(size);
			}

			return b;
		}

		mem::Block ExtractBlock(void* block_ptr)
		{
			mem::Block block{};
			for (siz i = 0; i < _rbt_ptrs.size() && !block.IsValid(); i++)
				block = _rbt_ptrs[i]->ExtractBlock(block_ptr);

			return block;
		}

		mem::Block Reallocate(mem::Block& old_block, siz new_size) override
		{
			mem::Block new_block = Allocate(new_size);

			if (Contains(old_block))
			{
				mem::CopyBytes(new_block.Begin(), old_block.Begin(), old_block.size);
				Deallocate(old_block);
				old_block.Invalidate();
			}

			return new_block;
		}

		mem::Block Reallocate(void* old_ptr, siz new_size) override
		{
			mem::Block old_block = ExtractBlock(old_ptr);
			return Reallocate(old_block, new_size);
		}

		bl Deallocate(mem::Block& block) override
		{
			bl deallocated = Deallocate(block.ptr);
			if (deallocated)
				block.Invalidate();

			return deallocated;
		}

		bl Deallocate(void* ptr) override
		{
			bl deallocated = false;
			for (siz i = 0; i < _rbt_ptrs.size() && !deallocated; i++)
				deallocated = _rbt_ptrs[i]->Deallocate(ptr);

			return deallocated;
		}
	};
}

#endif /* NP_ENGINE_ACCUMULATING_ALLOCATOR_HPP */