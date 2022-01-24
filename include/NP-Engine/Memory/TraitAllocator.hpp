//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 7/20/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TRAIT_ALLOCATOR_HPP
#define NP_ENGINE_TRAIT_ALLOCATOR_HPP

#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"
#include "CAllocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::memory
{
	class TraitAllocator : public Allocator
	{
	private:
		static Allocator* _allocator;

	public:
		bl Contains(const Block& block) const override
		{
			return Contains(block.Begin());
		}

		bl Contains(const void* ptr) const override
		{
			NP_ASSERT(_allocator != nullptr, "Registered TraitAllocator is nullptr");
			return _allocator->Contains(ptr);
		}

		Block Allocate(siz size) override
		{
			NP_ASSERT(_allocator != nullptr, "Registered TraitAllocator is nullptr");
			return _allocator->Allocate(size);
		}

		bl Deallocate(Block& block) override
		{
			return Deallocate(block.ptr);
		}

		bl Deallocate(void* ptr) override
		{
			NP_ASSERT(_allocator != nullptr, "Registered TraitAllocator is nullptr");
			return _allocator->Deallocate(ptr);
		}

		static inline void* malloc(siz size)
		{
			NP_ASSERT(_allocator != nullptr, "Registered TraitAllocator is nullptr");
			return _allocator->Allocate(size).Begin();
		}

		static inline void free(void* ptr)
		{
			NP_ASSERT(_allocator != nullptr, "Registered TraitAllocator is nullptr");
			_allocator->Deallocate(ptr);
		}

		static inline void Register(Allocator& allocator)
		{
			_allocator = AddressOf(allocator);
		}
	};

	extern TraitAllocator DefaultTraitAllocator;
} // namespace np::memory

#endif /* NP_ENGINE_TRAIT_ALLOCATOR_HPP */
