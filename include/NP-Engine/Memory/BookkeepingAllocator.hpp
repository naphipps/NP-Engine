//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/19/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_BOOKKEEPING_ALLOCATOR_HPP
#define NP_ENGINE_BOOKKEEPING_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Block.hpp"
#include "BlockedAllocator.hpp"

namespace np::mem
{
	class BookkeepingAllocator : public BlockedAllocator
	{
	public:
		const static siz BOOKKEEPING_SIZE = 0;

		BookkeepingAllocator(Block block): BlockedAllocator(block) {}

		BookkeepingAllocator(siz size): BlockedAllocator(size) {}

		virtual Block Allocate(siz size) override = 0;

		virtual Block ExtractBlock(void* ptr) = 0;

		virtual Block Reallocate(Block& old_block, siz new_size) override = 0;

		virtual Block Reallocate(void* old_ptr, siz new_size) override = 0;

		virtual bl Deallocate(Block& block) override = 0;

		virtual bl Deallocate(void* ptr) override = 0;

		virtual bl DeallocateAll() override = 0;
	};
} // namespace np::mem

#endif /* NP_ENGINE_BOOKKEEPING_ALLOCATOR_HPP */