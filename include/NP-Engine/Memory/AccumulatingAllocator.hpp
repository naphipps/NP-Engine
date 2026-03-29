//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 9/6/22
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_MEM_ACCUMULATING_ALLOCATOR_HPP
#define NP_ENGINE_MEM_ACCUMULATING_ALLOCATOR_HPP

#ifndef NP_ENGINE_MEM_ACCUMULATING_ALLOCATOR_BLOCK_SIZE
	#define NP_ENGINE_MEM_ACCUMULATING_ALLOCATOR_BLOCK_SIZE GIGABYTE_SIZE
#endif

#include <vector>
#include <type_traits>

#include "MemoryFunctions.hpp"
#include "BookkeepingAllocator.hpp"

namespace np::mem
{
	template <typename ALLOCATOR_TYPE>
	class accumulating_allocator : public allocator
	{
	protected:
		NP_ENGINE_STATIC_ASSERT((::std::is_base_of_v<bookkeeping_allocator, ALLOCATOR_TYPE>),
								"ALLOCATOR_TYPE must derive from bookkeeping_allocator");

		constexpr static siz ALLOCATOR_TYPE_SIZE = calc_aligned_size(sizeof(ALLOCATOR_TYPE), DEFAULT_ALIGNMENT);

		NP_ENGINE_STATIC_ASSERT(ALLOCATOR_TYPE_SIZE < NP_ENGINE_MEM_ACCUMULATING_ALLOCATOR_BLOCK_SIZE,
								"make NP_ENGINE_APPLICATION_ALLOCATOR_BLOCK_SIZE larger");

		c_allocator _c_allocator;
		mutexed_wrapper<::std::vector<ALLOCATOR_TYPE*>> _allocators; //std::vector for convenience //TODO: I think we can remove this

	public:
		virtual ~accumulating_allocator()
		{
			deallocate_all();
		}

		virtual bl contains(const block& b) override
		{
			return contains(b.ptr);
		}

		virtual bl contains(const void* ptr) override
		{
			bl contains = false;
			auto allocators = _allocators.get_access();
			for (siz i = 0; i < allocators->size() && !contains; i++)
				contains = (*allocators)[i]->contains(ptr);
			return contains;
		}

		virtual block allocate(siz size, siz alignment) override
		{
			block b{};
			auto allocators = _allocators.get_access();
			for (siz i = 0; i < allocators->size() && !b.is_valid(); i++)
				b = (*allocators)[i]->allocate(size, alignment);

			if (!b.is_valid())
			{
				siz block_size = NP_ENGINE_MEM_ACCUMULATING_ALLOCATOR_BLOCK_SIZE;
				while (block_size - ALLOCATOR_TYPE_SIZE - ALLOCATOR_TYPE::get_overhead_size() <= size)
					block_size *= 2;

				block continguous_block = _c_allocator.allocate(block_size, DEFAULT_ALIGNMENT);
				block object_block{ continguous_block.ptr, ALLOCATOR_TYPE_SIZE};
				block allocate_block{object_block.end(), continguous_block.size - object_block.size};
				allocators->emplace_back(mem::construct<ALLOCATOR_TYPE>(object_block, allocate_block));
				b = allocators->back()->allocate(size, alignment);
			}

			return b;
		}

		virtual block extract_allocated_block(void* block_ptr)
		{
			block b{};
			auto allocators = _allocators.get_access();
			for (siz i = 0; i < allocators->size() && !b.is_valid(); i++)
				b = (*allocators)[i]->extract_allocated_block(block_ptr);
			return b;
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

		virtual block reallocate(void* ptr, siz size, siz alignment) override
		{
			block b = extract_allocated_block(ptr);
			return reallocate(b, size, alignment);
		}

		virtual bl deallocate(block& b) override
		{
			bl deallocated = deallocate(b.ptr);
			if (deallocated)
				b.invalidate();
			return deallocated;
		}

		virtual bl deallocate(void* ptr) override
		{
			bl deallocated = false;
			auto allocators = _allocators.get_access();
			for (siz i = 0; i < allocators->size() && !deallocated; i++)
				if ((*allocators)[i]->contains(ptr))
					deallocated = (*allocators)[i]->deallocate(ptr);
			return deallocated;
		}

		virtual bl deallocate_all()
		{
			auto allocators = _allocators.get_access();
			for (ALLOCATOR_TYPE* a : *allocators)
				mem::destroy<ALLOCATOR_TYPE>(_c_allocator, a);
			allocators->clear();
			return true;
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_MEM_ACCUMULATING_ALLOCATOR_HPP */