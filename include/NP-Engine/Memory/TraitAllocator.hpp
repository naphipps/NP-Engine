//##===----------------------------------------------------------------------===##//
//
//  Author: Nathan Phipps 7/20/21
//
//##===----------------------------------------------------------------------===##//

#ifndef NP_ENGINE_TRAIT_ALLOCATOR_HPP
#define NP_ENGINE_TRAIT_ALLOCATOR_HPP

#include "NP-Engine/Foundation/Foundation.hpp"
#include "NP-Engine/Primitive/Primitive.hpp"

#include "Allocator.hpp"
#include "CAllocator.hpp"
#include "MemoryFunctions.hpp"

namespace np::mem
{
	class TraitAllocator : public Allocator
	{
	private:
		static CAllocator _default_allocator;
		static atm<Allocator*> _registered_allocator;

		static void EnsureRegistration()
		{
			Allocator* expected = nullptr;
			_registered_allocator.compare_exchange_strong(expected, AddressOf(_default_allocator), mo_release, mo_relaxed);
			NP_ENGINE_ASSERT(expected, "TraitAllocator's registration is nullptr when it should not be");
		}

	public:
		bl Contains(const Block& block) override
		{
			return Contains(block.ptr);
		}

		bl Contains(const void* ptr) override
		{
			EnsureRegistration();
			return _registered_allocator.load(mo_acquire)->Contains(ptr);
		}

		Block Allocate(siz size) override
		{
			EnsureRegistration();
			return _registered_allocator.load(mo_acquire)->Allocate(size);
		}

		Block Reallocate(Block& old_block, siz new_size) override
		{
			EnsureRegistration();
			return _registered_allocator.load(mo_acquire)->Reallocate(old_block, new_size);
		}

		Block Reallocate(void* old_ptr, siz new_size) override
		{
			EnsureRegistration();
			return _registered_allocator.load(mo_acquire)->Reallocate(old_ptr, new_size);
		}

		bl Deallocate(Block& block) override
		{
			return Deallocate(block.ptr);
		}

		bl Deallocate(void* ptr) override
		{
			EnsureRegistration();
			return _registered_allocator.load(mo_acquire)->Deallocate(ptr);
		}

		static inline void* realloc(void* old_ptr, siz new_size)
		{
			EnsureRegistration();
			return _registered_allocator.load(mo_acquire)->Reallocate(old_ptr, new_size).ptr;
		}

		static inline void* malloc(siz size)
		{
			EnsureRegistration();
			return _registered_allocator.load(mo_acquire)->Allocate(size).ptr;
		}

		static inline void free(void* ptr)
		{
			EnsureRegistration();
			_registered_allocator.load(mo_acquire)->Deallocate(ptr);
		}

		static inline void Register(Allocator& allocator)
		{
			_registered_allocator.store(AddressOf(allocator), mo_release);
		}

		static inline void ResetRegistration()
		{
			Register(_default_allocator);
		}
	};
} // namespace np::mem

#endif /* NP_ENGINE_TRAIT_ALLOCATOR_HPP */
