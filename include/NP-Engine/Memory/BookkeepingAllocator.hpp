//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/19/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_BOOKKEEPING_ALLOCATOR_HPP
#define NP_ENGINE_MEM_BOOKKEEPING_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Block.hpp"
#include "BlockedAllocator.hpp"

namespace np::mem
{
	class bookkeeping_allocator : public blocked_allocator
	{
	public:
		static siz get_overhead_size()
		{
			return 0;
		}

		bookkeeping_allocator(block b): blocked_allocator(b) {}

		bookkeeping_allocator(siz size, siz alignment): blocked_allocator(size, alignment) {}

		virtual ~bookkeeping_allocator() = default;

		virtual block allocate(siz size, siz alignment) override = 0;

		virtual block extract_allocated_block(void* ptr) = 0;

		virtual block reallocate(block& b, siz size, siz alignment) override = 0;

		virtual block reallocate(void* ptr, siz size, siz alignment) override = 0;

		virtual bl deallocate(block& b) override = 0;

		virtual bl deallocate(void* ptr) override = 0;

		virtual bl deallocate_all() override = 0;
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_BOOKKEEPING_ALLOCATOR_HPP */